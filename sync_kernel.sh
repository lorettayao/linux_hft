#!/bin/bash
# 自動從 linux 資料夾同步改動過的檔案到備份目錄
K_SRC=~/linux
BACKUP_DIR=~/hft-sandbox/kernel_mods

cp $K_SRC/kernel/sys.c $BACKUP_DIR/
cp $K_SRC/arch/x86/entry/syscalls/syscall_64.tbl $BACKUP_DIR/
cp $K_SRC/scripts/Makefile.build $BACKUP_DIR/

echo "Kernel files synced to backup directory!"
