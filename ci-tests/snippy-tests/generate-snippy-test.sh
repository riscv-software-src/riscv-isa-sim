#!/usr/bin/env bash

set -euo pipefail

args=("$@")

march=
mtriple=
extensions=
ignore_regex='^$'
includes=("./sections.yaml")
num_instrs=1000

usage() {
    {
        echo "isa-tests-gen.sh [options]"
        echo "    --march               : Target architecture [required]"
        echo "    --mtriple             : Target triple"
        echo "    --extensions          : Target extensions string"
        echo "    --include             : Additional include"
        echo "    --num-instrs          : Number of instructions to generate (default: $num_instrs)"
        echo "    --ignore-opcode-regex : Regex to filter out opcodes"
        echo "    -h, --help            : Print this help message"
    } >&2
}

while [[ $# -gt 0 ]]; do
    case $1 in
    --march)
        shift
        march=$1
        shift
        ;;
    --mtriple)
        shift
        mtriple=$1
        shift
        ;;
    --extensions)
        shift
        extensions="$1"
        shift
        ;;
    --include)
        shift
        includes+=("$1")
        shift
        ;;
    --num-instrs)
        shift
        num_instrs="$1"
        shift
        ;;
    --ignore-opcode-regex)
        shift
        ignore_regex="$1"
        shift
        ;;
    -h | --help)
        usage
        exit 0
        ;;
    *)
        usage
        exit 1
        ;;
    esac
done

declare -a ie_args

case $mtriple in
riscv32)
    ie_args+=("-arch=riscv" "--rv32" "-riscv-ext" "$extensions")
    ;;
riscv64)
    ie_args+=("-arch=riscv" "--rv64" "-riscv-ext" "$extensions")
    ;;
*)
    echo "error: Unrecognized --mtriple" >&2
    usage
    exit 1
    ;;
esac

ie_args+=("--disable-pseudo")

if [[ -z "$march" ]]; then
    echo "error: --march hasn't been specified" >&2
    exit 1
fi

if ! [ -x "$(command -v llvm-ie)" ]; then
    echo "error: 'llvm-ie' is not in PATH" >&2
    exit 1
fi

mapfile -t opcodes < <(llvm-ie "${ie_args[@]}")
filtered_opcodes=()

for opc in "${opcodes[@]}"; do
    if [[ ! "$opc" =~ $ignore_regex ]]; then
        filtered_opcodes+=("$opc")
    fi
done

if [[ ${#filtered_opcodes[@]} -eq 0 ]]; then
    echo "error: No opcodes matched" >&2
    exit 1
fi

echo "# generated with" "$(basename "$0"), to regenerate run:"
printf "# %s" "$(basename "$0")"
for arg in "${args[@]}"; do
    printf " %q" "$arg"
done
printf "\n"
echo "include:"
for inc in "${includes[@]}"; do
    echo "  - \"$inc\""
done

cat <<EOF
options:
  march: ${march}
  mtriple: ${mtriple}
  num-instrs: ${num_instrs}
  model-plugin: None
  entry-point: SNIPPY_ENTRY
  riscv-init-fregs-from-memory: true
histogram:
EOF
for opc in "${filtered_opcodes[@]}"; do
    echo "  - [$opc, 1.0]"
done

printf "\n"
