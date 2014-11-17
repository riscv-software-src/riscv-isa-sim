/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <errno.h>
#include "interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

#ifdef EACCES
static int EACCESValue = EACCES;
#endif

#ifdef EADDRINUSE
static int EADDRINUSEValue = EADDRINUSE;
#endif

#ifdef EADDRNOTAVAIL
static int EADDRNOTAVAILValue = EADDRNOTAVAIL;
#endif

#ifdef EAFNOSUPPORT
static int EAFNOSUPPORTValue = EAFNOSUPPORT;
#endif

#ifdef EAGAIN
static int EAGAINValue = EAGAIN;
#endif

#ifdef EALREADY
static int EALREADYValue = EALREADY;
#endif

#ifdef EBADF
static int EBADFValue = EBADF;
#endif

#ifdef EBADMSG
static int EBADMSGValue = EBADMSG;
#endif

#ifdef EBUSY
static int EBUSYValue = EBUSY;
#endif

#ifdef ECANCELED
static int ECANCELEDValue = ECANCELED;
#endif

#ifdef ECHILD
static int ECHILDValue = ECHILD;
#endif

#ifdef ECONNABORTED
static int ECONNABORTEDValue = ECONNABORTED;
#endif

#ifdef ECONNREFUSED
static int ECONNREFUSEDValue = ECONNREFUSED;
#endif

#ifdef ECONNRESET
static int ECONNRESETValue = ECONNRESET;
#endif

#ifdef EDEADLK
static int EDEADLKValue = EDEADLK;
#endif

#ifdef EDESTADDRREQ
static int EDESTADDRREQValue = EDESTADDRREQ;
#endif

#ifdef EDOM
static int EDOMValue = EDOM;
#endif

#ifdef EDQUOT
static int EDQUOTValue = EDQUOT;
#endif

#ifdef EEXIST
static int EEXISTValue = EEXIST;
#endif

#ifdef EFAULT
static int EFAULTValue = EFAULT;
#endif

#ifdef EFBIG
static int EFBIGValue = EFBIG;
#endif

#ifdef EHOSTUNREACH
static int EHOSTUNREACHValue = EHOSTUNREACH;
#endif

#ifdef EIDRM
static int EIDRMValue = EIDRM;
#endif

#ifdef EILSEQ
static int EILSEQValue = EILSEQ;
#endif

#ifdef EINPROGRESS
static int EINPROGRESSValue = EINPROGRESS;
#endif

#ifdef EINTR
static int EINTRValue = EINTR;
#endif

#ifdef EINVAL
static int EINVALValue = EINVAL;
#endif

#ifdef EIO
static int EIOValue = EIO;
#endif

#ifdef EISCONN
static int EISCONNValue = EISCONN;
#endif

#ifdef EISDIR
static int EISDIRValue = EISDIR;
#endif

#ifdef ELOOP
static int ELOOPValue = ELOOP;
#endif

#ifdef EMFILE
static int EMFILEValue = EMFILE;
#endif

#ifdef EMLINK
static int EMLINKValue = EMLINK;
#endif

#ifdef EMSGSIZE
static int EMSGSIZEValue = EMSGSIZE;
#endif

#ifdef EMULTIHOP
static int EMULTIHOPValue = EMULTIHOP;
#endif

#ifdef ENAMETOOLONG
static int ENAMETOOLONGValue = ENAMETOOLONG;
#endif

#ifdef ENETDOWN
static int ENETDOWNValue = ENETDOWN;
#endif

#ifdef ENETRESET
static int ENETRESETValue = ENETRESET;
#endif

#ifdef ENETUNREACH
static int ENETUNREACHValue = ENETUNREACH;
#endif

#ifdef ENFILE
static int ENFILEValue = ENFILE;
#endif

#ifdef ENOBUFS
static int ENOBUFSValue = ENOBUFS;
#endif

#ifdef ENODATA
static int ENODATAValue = ENODATA;
#endif

#ifdef ENODEV
static int ENODEVValue = ENODEV;
#endif

#ifdef ENOENT
static int ENOENTValue = ENOENT;
#endif

#ifdef ENOEXEC
static int ENOEXECValue = ENOEXEC;
#endif

#ifdef ENOLCK
static int ENOLCKValue = ENOLCK;
#endif

#ifdef ENOLINK
static int ENOLINKValue = ENOLINK;
#endif

#ifdef ENOMEM
static int ENOMEMValue = ENOMEM;
#endif

#ifdef ENOMSG
static int ENOMSGValue = ENOMSG;
#endif

#ifdef ENOPROTOOPT
static int ENOPROTOOPTValue = ENOPROTOOPT;
#endif

#ifdef ENOSPC
static int ENOSPCValue = ENOSPC;
#endif

#ifdef ENOSR
static int ENOSRValue = ENOSR;
#endif

#ifdef ENOSTR
static int ENOSTRValue = ENOSTR;
#endif

#ifdef ENOSYS
static int ENOSYSValue = ENOSYS;
#endif

#ifdef ENOTCONN
static int ENOTCONNValue = ENOTCONN;
#endif

#ifdef ENOTDIR
static int ENOTDIRValue = ENOTDIR;
#endif

#ifdef ENOTEMPTY
static int ENOTEMPTYValue = ENOTEMPTY;
#endif

#ifdef ENOTRECOVERABLE
static int ENOTRECOVERABLEValue = ENOTRECOVERABLE;
#endif

#ifdef ENOTSOCK
static int ENOTSOCKValue = ENOTSOCK;
#endif

#ifdef ENOTSUP
static int ENOTSUPValue = ENOTSUP;
#endif

#ifdef ENOTTY
static int ENOTTYValue = ENOTTY;
#endif

#ifdef ENXIO
static int ENXIOValue = ENXIO;
#endif

#ifdef EOPNOTSUPP
static int EOPNOTSUPPValue = EOPNOTSUPP;
#endif

#ifdef EOVERFLOW
static int EOVERFLOWValue = EOVERFLOW;
#endif

#ifdef EOWNERDEAD
static int EOWNERDEADValue = EOWNERDEAD;
#endif

#ifdef EPERM
static int EPERMValue = EPERM;
#endif

#ifdef EPIPE
static int EPIPEValue = EPIPE;
#endif

#ifdef EPROTO
static int EPROTOValue = EPROTO;
#endif

#ifdef EPROTONOSUPPORT
static int EPROTONOSUPPORTValue = EPROTONOSUPPORT;
#endif

#ifdef EPROTOTYPE
static int EPROTOTYPEValue = EPROTOTYPE;
#endif

#ifdef ERANGE
static int ERANGEValue = ERANGE;
#endif

#ifdef EROFS
static int EROFSValue = EROFS;
#endif

#ifdef ESPIPE
static int ESPIPEValue = ESPIPE;
#endif

#ifdef ESRCH
static int ESRCHValue = ESRCH;
#endif

#ifdef ESTALE
static int ESTALEValue = ESTALE;
#endif

#ifdef ETIME
static int ETIMEValue = ETIME;
#endif

#ifdef ETIMEDOUT
static int ETIMEDOUTValue = ETIMEDOUT;
#endif

#ifdef ETXTBSY
static int ETXTBSYValue = ETXTBSY;
#endif

#ifdef EWOULDBLOCK
static int EWOULDBLOCKValue = EWOULDBLOCK;
#endif

#ifdef EXDEV
static int EXDEVValue = EXDEV;
#endif


/* creates various system-dependent definitions */
void StdErrnoSetupFunc(void)
{
    /* defines */
#ifdef EACCES
    VariableDefinePlatformVar(NULL, "EACCES", &IntType, (union AnyValue *)&EACCESValue, FALSE);
#endif

#ifdef EADDRINUSE
    VariableDefinePlatformVar(NULL, "EADDRINUSE", &IntType, (union AnyValue *)&EADDRINUSEValue, FALSE);
#endif

#ifdef EADDRNOTAVAIL
    VariableDefinePlatformVar(NULL, "EADDRNOTAVAIL", &IntType, (union AnyValue *)&EADDRNOTAVAILValue, FALSE);
#endif

#ifdef EAFNOSUPPORT
    VariableDefinePlatformVar(NULL, "EAFNOSUPPORT", &IntType, (union AnyValue *)&EAFNOSUPPORTValue, FALSE);
#endif

#ifdef EAGAIN
    VariableDefinePlatformVar(NULL, "EAGAIN", &IntType, (union AnyValue *)&EAGAINValue, FALSE);
#endif

#ifdef EALREADY
    VariableDefinePlatformVar(NULL, "EALREADY", &IntType, (union AnyValue *)&EALREADYValue, FALSE);
#endif

#ifdef EBADF
    VariableDefinePlatformVar(NULL, "EBADF", &IntType, (union AnyValue *)&EBADFValue, FALSE);
#endif

#ifdef EBADMSG
    VariableDefinePlatformVar(NULL, "EBADMSG", &IntType, (union AnyValue *)&EBADMSGValue, FALSE);
#endif

#ifdef EBUSY
    VariableDefinePlatformVar(NULL, "EBUSY", &IntType, (union AnyValue *)&EBUSYValue, FALSE);
#endif

#ifdef ECANCELED
    VariableDefinePlatformVar(NULL, "ECANCELED", &IntType, (union AnyValue *)&ECANCELEDValue, FALSE);
#endif

#ifdef ECHILD
    VariableDefinePlatformVar(NULL, "ECHILD", &IntType, (union AnyValue *)&ECHILDValue, FALSE);
#endif

#ifdef ECONNABORTED
    VariableDefinePlatformVar(NULL, "ECONNABORTED", &IntType, (union AnyValue *)&ECONNABORTEDValue, FALSE);
#endif

#ifdef ECONNREFUSED
    VariableDefinePlatformVar(NULL, "ECONNREFUSED", &IntType, (union AnyValue *)&ECONNREFUSEDValue, FALSE);
#endif

#ifdef ECONNRESET
    VariableDefinePlatformVar(NULL, "ECONNRESET", &IntType, (union AnyValue *)&ECONNRESETValue, FALSE);
#endif

#ifdef EDEADLK
    VariableDefinePlatformVar(NULL, "EDEADLK", &IntType, (union AnyValue *)&EDEADLKValue, FALSE);
#endif

#ifdef EDESTADDRREQ
    VariableDefinePlatformVar(NULL, "EDESTADDRREQ", &IntType, (union AnyValue *)&EDESTADDRREQValue, FALSE);
#endif

#ifdef EDOM
    VariableDefinePlatformVar(NULL, "EDOM", &IntType, (union AnyValue *)&EDOMValue, FALSE);
#endif

#ifdef EDQUOT
    VariableDefinePlatformVar(NULL, "EDQUOT", &IntType, (union AnyValue *)&EDQUOTValue, FALSE);
#endif

#ifdef EEXIST
    VariableDefinePlatformVar(NULL, "EEXIST", &IntType, (union AnyValue *)&EEXISTValue, FALSE);
#endif

#ifdef EFAULT
    VariableDefinePlatformVar(NULL, "EFAULT", &IntType, (union AnyValue *)&EFAULTValue, FALSE);
#endif

#ifdef EFBIG
    VariableDefinePlatformVar(NULL, "EFBIG", &IntType, (union AnyValue *)&EFBIGValue, FALSE);
#endif

#ifdef EHOSTUNREACH
    VariableDefinePlatformVar(NULL, "EHOSTUNREACH", &IntType, (union AnyValue *)&EHOSTUNREACHValue, FALSE);
#endif

#ifdef EIDRM
    VariableDefinePlatformVar(NULL, "EIDRM", &IntType, (union AnyValue *)&EIDRMValue, FALSE);
#endif

#ifdef EILSEQ
    VariableDefinePlatformVar(NULL, "EILSEQ", &IntType, (union AnyValue *)&EILSEQValue, FALSE);
#endif

#ifdef EINPROGRESS
    VariableDefinePlatformVar(NULL, "EINPROGRESS", &IntType, (union AnyValue *)&EINPROGRESSValue, FALSE);
#endif

#ifdef EINTR
    VariableDefinePlatformVar(NULL, "EINTR", &IntType, (union AnyValue *)&EINTRValue, FALSE);
#endif

#ifdef EINVAL
    VariableDefinePlatformVar(NULL, "EINVAL", &IntType, (union AnyValue *)&EINVALValue, FALSE);
#endif

#ifdef EIO
    VariableDefinePlatformVar(NULL, "EIO", &IntType, (union AnyValue *)&EIOValue, FALSE);
#endif

#ifdef EISCONN
    VariableDefinePlatformVar(NULL, "EISCONN", &IntType, (union AnyValue *)&EISCONNValue, FALSE);
#endif

#ifdef EISDIR
    VariableDefinePlatformVar(NULL, "EISDIR", &IntType, (union AnyValue *)&EISDIRValue, FALSE);
#endif

#ifdef ELOOP
    VariableDefinePlatformVar(NULL, "ELOOP", &IntType, (union AnyValue *)&ELOOPValue, FALSE);
#endif

#ifdef EMFILE
    VariableDefinePlatformVar(NULL, "EMFILE", &IntType, (union AnyValue *)&EMFILEValue, FALSE);
#endif

#ifdef EMLINK
    VariableDefinePlatformVar(NULL, "EMLINK", &IntType, (union AnyValue *)&EMLINKValue, FALSE);
#endif

#ifdef EMSGSIZE
    VariableDefinePlatformVar(NULL, "EMSGSIZE", &IntType, (union AnyValue *)&EMSGSIZEValue, FALSE);
#endif

#ifdef EMULTIHOP
    VariableDefinePlatformVar(NULL, "EMULTIHOP", &IntType, (union AnyValue *)&EMULTIHOPValue, FALSE);
#endif

#ifdef ENAMETOOLONG
    VariableDefinePlatformVar(NULL, "ENAMETOOLONG", &IntType, (union AnyValue *)&ENAMETOOLONGValue, FALSE);
#endif

#ifdef ENETDOWN
    VariableDefinePlatformVar(NULL, "ENETDOWN", &IntType, (union AnyValue *)&ENETDOWNValue, FALSE);
#endif

#ifdef ENETRESET
    VariableDefinePlatformVar(NULL, "ENETRESET", &IntType, (union AnyValue *)&ENETRESETValue, FALSE);
#endif

#ifdef ENETUNREACH
    VariableDefinePlatformVar(NULL, "ENETUNREACH", &IntType, (union AnyValue *)&ENETUNREACHValue, FALSE);
#endif

#ifdef ENFILE
    VariableDefinePlatformVar(NULL, "ENFILE", &IntType, (union AnyValue *)&ENFILEValue, FALSE);
#endif

#ifdef ENOBUFS
    VariableDefinePlatformVar(NULL, "ENOBUFS", &IntType, (union AnyValue *)&ENOBUFSValue, FALSE);
#endif

#ifdef ENODATA
    VariableDefinePlatformVar(NULL, "ENODATA", &IntType, (union AnyValue *)&ENODATAValue, FALSE);
#endif

#ifdef ENODEV
    VariableDefinePlatformVar(NULL, "ENODEV", &IntType, (union AnyValue *)&ENODEVValue, FALSE);
#endif

#ifdef ENOENT
    VariableDefinePlatformVar(NULL, "ENOENT", &IntType, (union AnyValue *)&ENOENTValue, FALSE);
#endif

#ifdef ENOEXEC
    VariableDefinePlatformVar(NULL, "ENOEXEC", &IntType, (union AnyValue *)&ENOEXECValue, FALSE);
#endif

#ifdef ENOLCK
    VariableDefinePlatformVar(NULL, "ENOLCK", &IntType, (union AnyValue *)&ENOLCKValue, FALSE);
#endif

#ifdef ENOLINK
    VariableDefinePlatformVar(NULL, "ENOLINK", &IntType, (union AnyValue *)&ENOLINKValue, FALSE);
#endif

#ifdef ENOMEM
    VariableDefinePlatformVar(NULL, "ENOMEM", &IntType, (union AnyValue *)&ENOMEMValue, FALSE);
#endif

#ifdef ENOMSG
    VariableDefinePlatformVar(NULL, "ENOMSG", &IntType, (union AnyValue *)&ENOMSGValue, FALSE);
#endif

#ifdef ENOPROTOOPT
    VariableDefinePlatformVar(NULL, "ENOPROTOOPT", &IntType, (union AnyValue *)&ENOPROTOOPTValue, FALSE);
#endif

#ifdef ENOSPC
    VariableDefinePlatformVar(NULL, "ENOSPC", &IntType, (union AnyValue *)&ENOSPCValue, FALSE);
#endif

#ifdef ENOSR
    VariableDefinePlatformVar(NULL, "ENOSR", &IntType, (union AnyValue *)&ENOSRValue, FALSE);
#endif

#ifdef ENOSTR
    VariableDefinePlatformVar(NULL, "ENOSTR", &IntType, (union AnyValue *)&ENOSTRValue, FALSE);
#endif

#ifdef ENOSYS
    VariableDefinePlatformVar(NULL, "ENOSYS", &IntType, (union AnyValue *)&ENOSYSValue, FALSE);
#endif

#ifdef ENOTCONN
    VariableDefinePlatformVar(NULL, "ENOTCONN", &IntType, (union AnyValue *)&ENOTCONNValue, FALSE);
#endif

#ifdef ENOTDIR
    VariableDefinePlatformVar(NULL, "ENOTDIR", &IntType, (union AnyValue *)&ENOTDIRValue, FALSE);
#endif

#ifdef ENOTEMPTY
    VariableDefinePlatformVar(NULL, "ENOTEMPTY", &IntType, (union AnyValue *)&ENOTEMPTYValue, FALSE);
#endif

#ifdef ENOTRECOVERABLE
    VariableDefinePlatformVar(NULL, "ENOTRECOVERABLE", &IntType, (union AnyValue *)&ENOTRECOVERABLEValue, FALSE);
#endif

#ifdef ENOTSOCK
    VariableDefinePlatformVar(NULL, "ENOTSOCK", &IntType, (union AnyValue *)&ENOTSOCKValue, FALSE);
#endif

#ifdef ENOTSUP
    VariableDefinePlatformVar(NULL, "ENOTSUP", &IntType, (union AnyValue *)&ENOTSUPValue, FALSE);
#endif

#ifdef ENOTTY
    VariableDefinePlatformVar(NULL, "ENOTTY", &IntType, (union AnyValue *)&ENOTTYValue, FALSE);
#endif

#ifdef ENXIO
    VariableDefinePlatformVar(NULL, "ENXIO", &IntType, (union AnyValue *)&ENXIOValue, FALSE);
#endif

#ifdef EOPNOTSUPP
    VariableDefinePlatformVar(NULL, "EOPNOTSUPP", &IntType, (union AnyValue *)&EOPNOTSUPPValue, FALSE);
#endif

#ifdef EOVERFLOW
    VariableDefinePlatformVar(NULL, "EOVERFLOW", &IntType, (union AnyValue *)&EOVERFLOWValue, FALSE);
#endif

#ifdef EOWNERDEAD
    VariableDefinePlatformVar(NULL, "EOWNERDEAD", &IntType, (union AnyValue *)&EOWNERDEADValue, FALSE);
#endif

#ifdef EPERM
    VariableDefinePlatformVar(NULL, "EPERM", &IntType, (union AnyValue *)&EPERMValue, FALSE);
#endif

#ifdef EPIPE
    VariableDefinePlatformVar(NULL, "EPIPE", &IntType, (union AnyValue *)&EPIPEValue, FALSE);
#endif

#ifdef EPROTO
    VariableDefinePlatformVar(NULL, "EPROTO", &IntType, (union AnyValue *)&EPROTOValue, FALSE);
#endif

#ifdef EPROTONOSUPPORT
    VariableDefinePlatformVar(NULL, "EPROTONOSUPPORT", &IntType, (union AnyValue *)&EPROTONOSUPPORTValue, FALSE);
#endif

#ifdef EPROTOTYPE
    VariableDefinePlatformVar(NULL, "EPROTOTYPE", &IntType, (union AnyValue *)&EPROTOTYPEValue, FALSE);
#endif

#ifdef ERANGE
    VariableDefinePlatformVar(NULL, "ERANGE", &IntType, (union AnyValue *)&ERANGEValue, FALSE);
#endif

#ifdef EROFS
    VariableDefinePlatformVar(NULL, "EROFS", &IntType, (union AnyValue *)&EROFSValue, FALSE);
#endif

#ifdef ESPIPE
    VariableDefinePlatformVar(NULL, "ESPIPE", &IntType, (union AnyValue *)&ESPIPEValue, FALSE);
#endif

#ifdef ESRCH
    VariableDefinePlatformVar(NULL, "ESRCH", &IntType, (union AnyValue *)&ESRCHValue, FALSE);
#endif

#ifdef ESTALE
    VariableDefinePlatformVar(NULL, "ESTALE", &IntType, (union AnyValue *)&ESTALEValue, FALSE);
#endif

#ifdef ETIME
    VariableDefinePlatformVar(NULL, "ETIME", &IntType, (union AnyValue *)&ETIMEValue, FALSE);
#endif

#ifdef ETIMEDOUT
    VariableDefinePlatformVar(NULL, "ETIMEDOUT", &IntType, (union AnyValue *)&ETIMEDOUTValue, FALSE);
#endif

#ifdef ETXTBSY
    VariableDefinePlatformVar(NULL, "ETXTBSY", &IntType, (union AnyValue *)&ETXTBSYValue, FALSE);
#endif

#ifdef EWOULDBLOCK
    VariableDefinePlatformVar(NULL, "EWOULDBLOCK", &IntType, (union AnyValue *)&EWOULDBLOCKValue, FALSE);
#endif

#ifdef EXDEV
    VariableDefinePlatformVar(NULL, "EXDEV", &IntType, (union AnyValue *)&EXDEVValue, FALSE);
#endif

    VariableDefinePlatformVar(NULL, "errno", &IntType, (union AnyValue *)&errno, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */
