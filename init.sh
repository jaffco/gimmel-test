git submodule update --init --recursive

mkdir build

echo "Generating test audio file"

python media/wavToArray.py media/test.wav

echo "Initialization Complete"