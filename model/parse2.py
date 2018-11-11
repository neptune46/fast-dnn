import tensorflow as tf

def create_graph(modelFullPath):
    """Creates a graph from saved GraphDef file and returns a saver."""
    # Creates graph from saved graph_def.pb.
    with tf.gfile.FastGFile(modelFullPath, 'rb') as f:
        graph_def = tf.GraphDef()
        graph_def.ParseFromString(f.read())
        tf.import_graph_def(graph_def, name='')

GRAPH_DIR='m.pb'
create_graph(GRAPH_DIR)

constant_values = {}

with tf.Session() as sess:
  constant_ops = [op for op in sess.graph.get_operations() if op.type == "Const"]
  for constant_op in constant_ops:
    constant_values[constant_op.name] = sess.run(constant_op.outputs[0])

print('done')