# generate tensorflow models

## setup environment
```cmd
pip install tensorflow keras
```

## generate models

### generate pb model using tensorflow directly
```cmd
python test.py
```

### generate h5 format model firstly, then convert to pb format model
```cmd
python test2.py
python keras_to_tensorflow.py --input_model data/mnist.h5 --output_model=data/mnist.pb
```

## parse weights/bias from model files
```cmd
python parse.py
python parse2.py
```
