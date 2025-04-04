from neuronClass import *

# Instantiate presynaptic neuron
preNeuron1 = MultipolarNeuron()

# Instantiate intermediary neuron
intermedNeuron1 = MultipolarNeuron()

# Instantiate post-synaptic neuron
postNeuron1 = MultipolarNeuron()

# Info: 
print(preNeuron1.describe() 
      + "\n\n"
      + intermedNeuron1.describe()
      + "\n\n" 
      + postNeuron1.describe())

