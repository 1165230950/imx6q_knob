
function build_dir()
{
	echo -e "\033[44;37mmake $1 \033[0m"
		cd  $1
		make 

		if [ $? = 0 ]; then
			echo
		else
			echo -e "\033[41;37mmake $1 error!!\033[0m"
				make | grep -C 1 "error:"
				echo -e "\033[41;37mmake $1 error!!\033[0m"
				read get_key
		fi

}
clear

mkdir -pm 777 $DBG_OUT_PATH

build_dir $DBG_APP_PATH/test_app

