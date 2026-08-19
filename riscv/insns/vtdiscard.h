require_extension(EXT_ZVTBASE);
require_vector(false);
require(STATE.sstatus->enabled(SSTATUS_MS));
require(P.VU.vstart->read() == 0);

// set mstatus.MS to Initial
STATE.mstatus->write((STATE.mstatus->read() & ~SSTATUS_MS) | (SSTATUS_MS & (SSTATUS_MS >> 1)));
// set vsstatus.MS to Initial if V=1
STATE.sstatus->write((STATE.sstatus->read() & ~SSTATUS_MS) | (SSTATUS_MS & (SSTATUS_MS >> 1)));

VECTOR_END;
