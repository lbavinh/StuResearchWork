#!/bin/bash
WORKING_DIR=/weekly/$USER/lbavinh/vHLLE+UrQMD
rm -f ${WORKING_DIR}/OUT/log/*
rm -f ${WORKING_DIR}/OUT/*/log/*
rm -f ${WORKING_DIR}/OUT/*/sum_*_*.root
rm -f ${WORKING_DIR}/FlowANA_C*
rm -f ${WORKING_DIR}/anaFlow_C*
rm -f ${WORKING_DIR}/calculateFlow_C*
rm -f ${WORKING_DIR}/test.root