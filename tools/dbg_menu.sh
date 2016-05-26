function dbg_ko_install()
{
	echo $FUNCNAME
	echo 安装ko到文件系统
	cp $DBG_COPY_DIR/*.ko $DBG_FS_MODULES
}
function lidbg_clean()
{
        echo $FUNCNAME
        echo 清除生成文件
        cd $DBG_BUILD_PATH && ./clean.sh
}

function lidbg_build()
{
        echo $FUNCNAME
        echo 编译模块
	#if [ "$WORK_REMOTE" = 1 ];then
        #	expect $DBG_TOOLS_PATH/scp $WORK_REMOTE_USERNAME $WORK_REMOTE_PASSWORD $WORK_LOCAL_PATH $WORK_REMOTE_PATH/..
	#fi
        cd $DBG_BUILD_PATH && ./build.sh
}
function build_app()
{
	echo $FUNCNAME
	echo 编译应用
	cd $DBG_APP_BUILD_PATH && ./build.sh
}

function lidbg_handle()
{
	cd $DBG_ROOT_PATH
	case $1 in
	1)
		lidbg_clean;;
	2)
		lidbg_build;;
	3)
		dbg_ko_install;;
	4)
		build_app;;
	esac
}

function menu_do()
{
	chmod 777 $DBG_ROOT_PATH -R
	if [[ $1 -le 20 ]] ;then
		lidbg_handle $1
	else
		echo 请输入有效数字
	fi
}

function lidbg_menu()
{
	echo $DBG_ROOT_PATH
	echo [1] clean'                        '清除生成文件	
	echo [2] build'                        '编译模块
	echo [3] install .ko to fs'            '安装ko文件到文件系统
	echo [4] build apps'                   '编译上层应用
}

function auto_build()
{
	       	menu_do $1 $2 $3 $4
		menu_do $2 $3 $4 $5
		menu_do $3 $4 $5
		menu_do $4 $5
		menu_do $5
		if [[ $2 -eq "-1" ]];then
			exit 1
		fi
	while :;do
		chmod 777 $DBG_ROOT_PATH -R
		cd $DBG_BUILD_PATH		
		lidbg_menu
		read -p "Enter your select:" name1 name2 name3 name4 name5
	       	menu_do $name1 $name2 $name3 $name4
		menu_do $name2 $name3 $name4 $name5
		menu_do $name3 $name4 $name5
		menu_do $name4 $name5
		menu_do $name5
	done
}
cd build/env
source dbg_env.sh
source imx_env.sh

auto_build $1 $2 $3 $4 $5;


