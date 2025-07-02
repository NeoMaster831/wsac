set(IS_DEBUGGER_PRESENT_ID "0x0")
set(REMOTE_DEBUGGER_PRESENT_ID "0x1")
set(NTQUERY_ID "0x2")
set(EBS_ID "0x3")
set(RTL_QUERY_HEAP_ID "0x4")
set(MANUAL_CHECK_HEAP_PROTECTION "0x5")
set(CHECK_KUSER_SHARED_USER "0x6")

################################################################################
# Signatures                                                                   #
################################################################################
string(RANDOM LENGTH 4 ALPHABET "0123456789ABCDEF" TEST_SIGNATURE)
set(TEST_SIGNATURE "0x${TEST_SIGNATURE}")
string(RANDOM LENGTH 4 ALPHABET "0123456789ABCDEF" REGULAR_SIGNATURE)
set(REGULAR_SIGNATURE "0x${REGULAR_SIGNATURE}")
string(RANDOM LENGTH 4 ALPHABET "0123456789ABCDEF" CHECKPOINT_SIGNATURE)
set(CHECKPOINT_SIGNATURE "0x${CHECKPOINT_SIGNATURE}")

################################################################################
# PSKs                                                                         #
################################################################################
set(PSK_LAYER1 "{")
foreach (i RANGE 1 32)
    string(RANDOM LENGTH 2 ALPHABET "0123456789ABCDEF" PSK_LAYER1_TERM)
    set(PSK_LAYER1 "${PSK_LAYER1} 0x${PSK_LAYER1_TERM},")
endforeach ()
set(PSK_LAYER1 "${PSK_LAYER1} }")

set(PSK_LAYER2 "{")
foreach (i RANGE 1 32)
    string(RANDOM LENGTH 2 ALPHABET "0123456789ABCDEF" PSK_LAYER1_TERM)
    set(PSK_LAYER2 "${PSK_LAYER2} 0x${PSK_LAYER1_TERM},")
endforeach ()
set(PSK_LAYER2 "${PSK_LAYER2} }")

################################################################################
# Preamble                                                                     #
################################################################################
set(PREAMBLE "{")
foreach (i RANGE 1 6)
    string(RANDOM LENGTH 2 ALPHABET "0123456789ABCDEF" PREAMBLE_TERM)
    set(PREAMBLE "${PREAMBLE} 0x${PREAMBLE_TERM},")
endforeach ()
set(PREAMBLE "${PREAMBLE} }")
