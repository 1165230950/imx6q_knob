src_dir=$DBG_SOC_PATH/$DBG_SOC
dest_dir=$DBG_OUT_PATH
cp -u $src_dir/*.ko     $dest_dir/      &> /dev/null

cd $DBG_DRIVERS_PATH
for each_dir in `ls -l | grep "^d" | awk '{print $NF}'`
	do
	src_dir=$DBG_DRIVERS_PATH/$each_dir
	cp -u $src_dir/*.ko     $dest_dir/      &> /dev/null
done

cd $DBG_CALL_PATH
for each_dir in `ls -l | grep "^d" | awk '{print $NF}'`
	do
	src_dir=$DBG_CALL_PATH/$each_dir
	cp -u $src_dir/*.ko     $dest_dir/      &> /dev/null
done

src_dir=$DBG_COM_API_PATH
cp -u $src_dir/*.ko     $dest_dir/      &> /dev/null

