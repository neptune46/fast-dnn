import tensorflow as tf
from tensorflow.python.platform import gfile
from tensorflow.python.framework import tensor_util

def saveList(f, l):
  for x in l:
    f.write(str(x)+", \n")

def save1D(f, l, name):
  dim = "["+ str(len(l)) + "]"
  f.write("static float " + name + dim + " = \n")
  f.write("{\n")
  saveList(f, l)
  f.write("};\n\n")

def save2D(f, ll, name):
  dim = "["+ str(len(ll)) + "]" + "["+ str(len(ll[0])) + "]"
  f.write("static float " + name + dim + " = {\n")
  for l in ll:
    f.write("{\n")
    saveList(f, l)
    f.write("},\n")
  f.write("};\n\n")

def dumpArray(name, array):
  a = name.split('/')
  arrayname = a[0] + "_" + a[1]
  filename = "model_data.h"
  l = array.tolist()
  with open(filename, 'a') as f:
    if len(array.shape) == 1:
      save1D(f, l, arrayname)
    elif len(array.shape) == 2:
      save2D(f, l, arrayname)
  print(arrayname)

GRAPH_PB_PATH = "mnist.pb" #path to your .pb file
with tf.Session(config=tf.ConfigProto(log_device_placement=True)) as sess:
  print("load graph")
  with gfile.FastGFile(GRAPH_PB_PATH,'rb') as f:
    graph_def = tf.GraphDef()
    graph_def.ParseFromString(f.read())
    sess.graph.as_default()
    tf.import_graph_def(graph_def, name='')
    graph_nodes=[n for n in graph_def.node]

    wts = [n for n in graph_nodes if n.op=='Const']

    for n in wts:
      print ("Name of the node - %s" % n.name)
      tensor1 = n.attr['value'].tensor
      array1 = tensor_util.MakeNdarray(tensor1)
      array2 = array1.transpose()
      if (array2.size > 1):
        dumpArray(n.name, array2)

print('done')