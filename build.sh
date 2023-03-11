build_option=(clean debug release)
test_option=(yes no)
sample_option=(yes no)

function help()
{
    echo "Usage : ./build.sh -build [build_option] -test [test_option] -sample [sample_option]"
    echo "build_option = clean debug release"
    echo "test_option = yes no"
    echo "sample_option = yes no"
    echo "example : ./build.sh -build debug -test yes -sample yes"
}

ret=0

function contains()
{
    ret=0
    local n=$#
    local value=${!n}
    for ((i=1;i<n;i++))
        do
            if [ "$value" == "${!i}" ]; then
                ret=1
                return
            fi
        done
}

if [ $# -lt 6 ]; then
    help
    exit
fi

while [ $# -gt 0 ]
    do
        if [ $1 == "-build" ]; then
            contains ${build_option[@]} $2
            if [ $ret != 1 ]; then
                echo "$2 is illegal build option"
                help
                exit 1
            fi
            buildOption=$2
            shift 2
        elif [ $1 == "-test" ]; then
            contains ${test_option[@]} $2
            if [ $ret != 1 ]; then
                echo "$2 is illegal test option"
                help
                exit 1
            fi
            testOption=$2
            shift 2
        elif [ $1 == "-sample" ]; then
            contains ${sample_option[@]} $2
            if [ $ret != 1 ]; then
                echo "$2 is illegal sample option"
                help
                exit 1
            fi
            sampleOption=$2
            shift 2
        else
            echo "Error : $1 is illegal option name"
            help
            exit 1
        fi
    done

if [ "$buildOption" == "clean" ]; then
    echo "build = $buildOption"
    echo "test = $testOption"
    echo "sample = $sampleOption"
    rm -rf build
    rm -rf lib
    rm -rf bin
    exit 1
fi

if [ ! -d "./build" ]; then
    mkdir build
fi

echo "build = $buildOption"
echo "test = $testOption"
echo "sample = $sampleOption"

cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=$buildOption -DTEST=$testOption -DSAMPLE=$sampleOption
make -j4
make install
