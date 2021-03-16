#include "qarma.h"

//int sbox_use = 0;
//text_t check_box[3] = { 0x3ee99a6c82af0c38, 0x9f5c41ec525603c9, 0xbcaf6c89de930765 };

//int sbox_use = 1;
//text_t check_box[3] = { 0x544b0ab95bda7c3a, 0xa512dd1e4e3ec582, 0xedf67ff370a483f2 };

int sbox_use = 2;
text_t check_box[3] = { 0xc003b93999b33765, 0x270a787275c48d10, 0x5c06a7501b63b2fd };


int m = MAX_LENGTH / 16;

const_t alpha = 0xC0AC29B7C97C50DD;
const_t c[8] = { 0x0000000000000000, 0x13198A2E03707344, 0xA4093822299F31D0, 0x082EFA98EC4E6C89,
				 0x452821E638D01377, 0xBE5466CF34E90C6C, 0x3F84D5B5B5470917, 0x9216D5D98979FB1B };

int sbox[3][16] = { { 0, 14,  2, 10,  9, 15,  8, 11,  6,  4,  3,  7, 13, 12,  1,  5},
					{10, 13, 14,  6, 15,  7,  3,  5,  9,  8,  0, 12, 11,  1,  2,  4},
					{11,  6,  8, 15, 12,  0,  9, 14,  3,  7,  4,  5, 13,  2,  1, 10} };

int sbox_inv[3][16] = { { 0, 14,  2, 10,  9, 15,  8, 11,  6,  4,  3,  7, 13, 12,  1,  5},
						{10, 13, 14,  6, 15,  7,  3,  5,  9,  8,  0, 12, 11,  1,  2,  4},
						{ 5, 14, 13,  8, 10, 11,  1,  9,  2,  6, 15,  0,  4, 12,  7,  3} };

int t[16] = { 0, 11,  6, 13, 10,  1, 12,  7,  5, 14,  3,  8, 15,  4,  9,  2 };
int t_inv[16] = { 0,  5, 15, 10, 13,  8,  2,  7, 11, 14,  4,  1,  6,  3,  9, 12 };
int h[16] = { 6,  5, 14, 15,  0,  1,  2,  3,  7, 12, 13,  4,  8,  9, 10, 11 };
int h_inv[16] = { 4,  5,  6,  7, 11,  1,  0,  8, 12, 13, 14, 15,  9, 10,  2,  3 };

#define Q M
#define M_inv M
cell_t M[16] = { 0, 1, 2, 1,
				 1, 0, 1, 2,
				 2, 1, 0, 1,
				 1, 2, 1, 0 };


void text2cell(cell_t* cell, text_t is) {
	// for 64 bits
	char* byte_ptr = (char*)&is;
	for (int i = 0; i < MAX_LENGTH / 8; i++) {
		char byte = byte_ptr[i];
		cell[2 * (7 - i) + 0] = (byte & 0xF0) >> 4;
		cell[2 * (7 - i) + 1] = byte & 0xF;
	}
}

text_t cell2text(cell_t* cell) {
	text_t is = 0;
	for (int i = 0; i < MAX_LENGTH / 8; i++) {
		text_t byte = 0;
		byte = (cell[2 * i] << 4) | cell[2 * i + 1];
		is = is | (byte << (7 - i) * 8UL);
	}
	return is;
}

text_t pseudo_reflect(text_t is, qkey_t tk) {
	cell_t cell[16];
	text2cell(cell, is);

	// ShuffleCells
	cell_t perm[16];
	for (int i = 0; i < 16; i++)
		perm[i] = cell[t[i]];

	// MixColumns
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			cell_t temp = 0;
			for (int j = 0; j < 4; j++) {
				cell_t b;
				if (b = Q[4 * x + j]) {
					cell_t a = perm[4 * j + y];
					temp ^= ((a << b) & 0x0F) | (a >> (4 - b));
				}
			}
			cell[4 * x + y] = temp;
		}
	}

	// AddRoundTweakey
	for (int i = 0; i < 16; i++)
		cell[i] ^= (tk >> (4 * (15 - i))) & 0xF;

	// ShuffleCells invert
	for (int i = 0; i < 16; i++)
		perm[i] = cell[t_inv[i]];

	return cell2text(perm);
}

text_t forward(text_t is, qkey_t tk, int r) {
	//printf("key = 0x%016llx\n", tk);

	is ^= tk;
	cell_t cell[16];
	text2cell(cell, is);
	//printf("0x%016llx\n", is);


	if (r != 0) {
		// ShuffleCells
		cell_t perm[16];
		for (int i = 0; i < 16; i++)
			perm[i] = cell[t[i]];

		// MixColumns
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				cell_t temp = 0;
				for (int j = 0; j < 4; j++) {
					cell_t b;
					if (b = M[4 * x + j]) {
						cell_t a = perm[4 * j + y];
						temp ^= ((a << b) & 0x0F) | (a >> (4 - b));
					}
				}
				cell[4 * x + y] = temp;
			}
		}
	}

	// SubCells
	for (int i = 0; i < 16; i++) {
		cell[i] = subcells[cell[i]];
	}
	is = cell2text(cell);
	//printf("0x%016llx\n", is);

	return is;
}

text_t backward(text_t is, qkey_t tk, int r) {
	cell_t cell[16];
	text2cell(cell, is);
	//printf("0x%016llx\n", is);

	// SubCells
	for (int i = 0; i < 16; i++) {
		cell[i] = subcells_inv[cell[i]];
	}
	//printf("key = 0x%016llx\n", tk);

	if (r != 0) {
		cell_t mixc[16];
		// MixColumns
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				cell_t temp = 0;
				for (int j = 0; j < 4; j++) {
					cell_t b;
					if (b = M_inv[4 * x + j]) {
						cell_t a = cell[4 * j + y];
						temp ^= ((a << b) & 0x0F) | (a >> (4 - b));
					}
				}
				mixc[4 * x + y] = temp;
			}
		}

		// ShuffleCells
		for (int i = 0; i < 16; i++)
			cell[i] = mixc[t_inv[i]];
	}

	is = cell2text(cell);
	is ^= tk;
	//printf("0x%016llx\n", is);
	return is;
}

cell_t LFSR(cell_t x) {
	cell_t b0 = (x >> 0) & 1;
	cell_t b1 = (x >> 1) & 1;
	cell_t b2 = (x >> 2) & 1;
	cell_t b3 = (x >> 3) & 1;

	return ((b0 ^ b1) << 3) | (b3 << 2) | (b2 << 1) | (b1 << 0);
}

cell_t LFSR_inv(cell_t x) {
	cell_t b0 = (x >> 0) & 1;
	cell_t b1 = (x >> 1) & 1;
	cell_t b2 = (x >> 2) & 1;
	cell_t b3 = (x >> 3) & 1;

	return ((b0 ^ b3) << 0) | (b0 << 1) | (b1 << 2) | (b2 << 3);
}

qkey_t forward_update_key(qkey_t T) {
	cell_t cell[16], temp[16];
	text2cell(cell, T);

	// h box
	for (int i = 0; i < 16; i++) {
		temp[i] = cell[h[i]];
	}

	// w LFSR
	temp[0] = LFSR(temp[0]);
	temp[1] = LFSR(temp[1]);
	temp[3] = LFSR(temp[3]);
	temp[4] = LFSR(temp[4]);
	temp[8] = LFSR(temp[8]);
	temp[11] = LFSR(temp[11]);
	temp[13] = LFSR(temp[13]);

	return cell2text(temp);
}

qkey_t backward_update_key(qkey_t T) {
	cell_t cell[16], temp[16];
	text2cell(cell, T);

	// w LFSR invert
	cell[0] = LFSR_inv(cell[0]);
	cell[1] = LFSR_inv(cell[1]);
	cell[3] = LFSR_inv(cell[3]);
	cell[4] = LFSR_inv(cell[4]);
	cell[8] = LFSR_inv(cell[8]);
	cell[11] = LFSR_inv(cell[11]);
	cell[13] = LFSR_inv(cell[13]);

	// h box
	for (int i = 0; i < 16; i++) {
		temp[i] = cell[h_inv[i]];
	}

	return cell2text(temp);
}

text_t qarma64_enc(text_t plaintext, tweak_t tweak, qkey_t w0, qkey_t k0, int rounds) {
	qkey_t w1 = ((w0 >> 1) | (w0 << (64 - 1))) ^ (w0 >> (16 * m - 1));
	qkey_t k1 = k0;

	text_t is = plaintext ^ w0;

	for (int i = 0; i < rounds; i++) {
		//printf("%d:\n", i);
		is = forward(is, k0 ^ tweak ^ c[i], i);
		tweak = forward_update_key(tweak);
	}

	//printf("0x%016llx\n", is);
	is = forward(is, w1 ^ tweak, 1);
	is = pseudo_reflect(is, k1);
	is = backward(is, w0 ^ tweak, 1);

	for (int i = rounds - 1; i >= 0; i--) {
		//printf("%d:\n", i);
		tweak = backward_update_key(tweak);
		is = backward(is, k0 ^ tweak ^ c[i] ^ alpha, i);
	}

	is ^= w1;
	//printf("0x%016llx\n", is);
	return is;
}

text_t qarma64_dec(text_t plaintext, tweak_t tweak, qkey_t w0, qkey_t k0, int rounds) {

	qkey_t w1 = w0;
	w0 = ((w0 >> 1) | (w0 << (64 - 1))) ^ (w0 >> (16 * m - 1));

	cell_t k0_cell[16], k1_cell[16];
	text2cell(k0_cell, k0);
	// MixColumns
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			cell_t temp = 0;
			for (int j = 0; j < 4; j++) {
				cell_t b;
				if (b = Q[4 * x + j]) {
					cell_t a = k0_cell[4 * j + y];
					temp ^= ((a << b) & 0x0F) | (a >> (4 - b));
				}
			}
			k1_cell[4 * x + y] = temp;
		}
	}
	qkey_t k1 = cell2text(k1_cell);

	k0 ^= alpha;

	text_t is = plaintext ^ w0;

	for (int i = 0; i < rounds; i++) {
		//printf("%d:\n", i);
		is = forward(is, k0 ^ tweak ^ c[i], i);
		tweak = forward_update_key(tweak);
	}

	//printf("0x%016llx\n", is);
	is = forward(is, w1 ^ tweak, 1);
	is = pseudo_reflect(is, k1);
	is = backward(is, w0 ^ tweak, 1);

	for (int i = rounds - 1; i >= 0; i--) {
		//printf("%d:\n", i);
		tweak = backward_update_key(tweak);
		is = backward(is, k0 ^ tweak ^ c[i] ^ alpha, i);
	}

	is ^= w1;
	//printf("0x%016llx\n", is);
	return is;
}
