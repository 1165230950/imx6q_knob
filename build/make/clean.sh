
cd $DBG_SOC_PATH/$DBG_SOC && make clean
cd $DBG_COM_API_PATH && make clean

dbg_soc=$DBG_DRIVERS_PATH/
cd $dbg_soc
for each_dir in `ls -l | grep "^d" | awk '{print $NF}'`
do
	cd $dbg_soc/$each_dir && make clean
done

if [ "$DBG_OUT_PATH" = "" ]; then
	echo "exit:  DBG_OUT_PATH=null"
	exit
fi

rm -rf $DBG_OUT_PATH
cd $DBG_ROOT_PATH
find . -name "*.o" -type f -exec rm  {} \;
find . -name "*.mod.c" -type f -exec rm  {} \;
find . -name "*.symvers" -type f -exec rm  {} \;
find . -name "*.order" -type f -exec rm  {} \;
find . -name "*.depend" -type f -exec rm  {} \;
find . -name "*~" -type f -exec rm  {} \;
find . -name "*tmp_versions" -type f -exec rm  {} \;
find . -name ".tmp_versions" -type d -exec rm  -rf {} \;
find . -name "*orig" -type f -exec rm  {} \;
find . -name "*.cmd" -type f -exec rm  {} \;
find . -name "*.order" -type f -exec rm  {} \;
find . -name "*.tmp" -type f -exec rm  {} \;

