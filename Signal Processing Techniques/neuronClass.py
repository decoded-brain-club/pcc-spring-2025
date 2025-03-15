# TO-DO: introduce controlled randomness for characteristics that are known to vary (dendrite diameter, soma volume, etc)
#        implement HH model for membrane in MultipolarNeuron
#        implement purkinje neurons, found in cerebellum
#        research tree data type

class Neuron:
    def describe(self):
        type_desc = "excitatory" if self.is_excitatory else "inhibitory"
        description = f"This is a {self.neuron_type} multipolar neuron ({type_desc}).\n"
        description += f"Soma diameter: {self.soma_diameter*1e6:.1f} μm\n"
        description += f"Dendrite count: {self.dendrite_count}\n"
        description += f"Axon length: {self.axon_length * 100:.1f} cm\n"
        description += f"Total synapses: approximately {self.synapse_count}\n"
        description += f"Neurotransmitter: {self.neurotransmitter}\n"
        description += f"Conduction velocity: {self.calculate_conduction_velocity():.1f} m/s"
        
        return description

class MultipolarNeuron(Neuron):
    """
    Class for multipolar neurons, typically found in the cerebral cortex.
    All physical measurements use SI units where applicable.
    """
    
    def __init__(self, neuron_type="pyramidal"):
        """
        Initializes a multipolar cortical neuron with default values.
        
        Parameters:
        neuron_type (str): Type of multipolar neuron ('pyramidal', 'stellate', etc.)
        """

        # Neuron classification
        self.neuron_type = neuron_type
        self.is_inhibitory = neuron_type in ["basket", "chandelier", "martinotti", "double-bouquet"]
        self.is_excitatory = not self.is_inhibitory
        
        # Soma characteristics
        self.soma_diameter = 20e-6          # meters (typical range: 10-30 μm)
        self.soma_volume = 4.2e-15          # cubic meters (typical range: 1-10 × 10^-15 m³)
        self.membrane_thickness = 5e-9      # meters (typical: 5 nm)
        
        # Dendrite characteristics
        self.dendrite_count = 7             # typical range: 5-12 for pyramidal neurons
        self.apical_dendrite_length = 650e-6 if neuron_type == "pyramidal" else 0  # meters
        self.basal_dendrite_length = 150e-6  # meters (per dendrite)
        self.dendrite_diameter = 2e-6       # meters (typical: 0.5-5 μm)
        self.dendritic_spine_density = 1.5  # spines per μm of dendrite length
        self.total_spine_count = int(self.dendritic_spine_density * 
                                   (self.apical_dendrite_length + 
                                    self.basal_dendrite_length * self.dendrite_count) * 1e6)
        
        # Axon characteristics
        self.axon_length = 0.08             # meters (highly variable, from mm to meters)
        self.axon_diameter = 1e-6           # meters (typical: 0.2-2 μm)
        self.axon_initial_segment_length = 25e-6  # meters
        self.axon_collateral_count = 40     # branches of the main axon
        self.synapse_count = 7000           # typical range: 5000-10000
        
        # Myelin characteristics 
        self.is_myelinated = True
        self.myelin_thickness = 0.2e-6      # meters (variable)
        self.internode_length = 100e-6      # meters (distance between nodes of Ranvier)
        self.node_of_ranvier_length = 1e-6  # meters
        
        # Electrical properties
        self.resting_potential = -70e-3             # volts
        self.action_potential_threshold = -55e-3    # volts
        self.action_potential_amplitude = 100e-3    # volts
        self.action_potential_duration = 1e-3       # seconds
        self.membrane_capacitance = 1e-2            # farads per square meter
        self.membrane_resistance = 1e8              # ohms * square meters
        self.axial_resistance = 1.5                 # ohms * meters
        
        # Neurotransmitter properties
        self.neurotransmitter = "glutamate" if self.is_excitatory else "GABA"
        self.neurotransmitter_vesicle_count = 300000  # per neuron
        self.vesicle_diameter = 40e-9                 # meters
        
        # Ion channels and receptors
        self.sodium_channel_density = 60            # channels per μm²
        self.potassium_channel_density = 18         # channels per μm²
        self.calcium_channel_density = 10           # channels per μm²
        
        # Metabolic properties
        self.mitochondria_count = 1000              # approximate
        self.glucose_consumption_rate = 5e-14       # moles per second
        self.atp_production_rate = 5e-13            # moles per second

        # inherit describe method from parent 
        super().describe()
    
    def calculate_conduction_velocity(self):
        """
        Calculate the approximate conduction velocity in meters per second.
        Conduction velocity is the speed at which an action potential travels through an axon.
        """
        # For myelinated axons, velocity scales with diameter
        if self.is_myelinated:
            return 6 * self.axon_diameter * 1e6  # Approximate formula: 6 × diameter in μm
        else:
            return 1.5  # Typical unmyelinated velocity in m/s
    
    def fire_action_potential(self, stimulus_strength):
        """
        Simulates firing an action potential.
        
        Parameters:
        stimulus_strength (float): Strength of stimulus in millivolts
        
        Returns:
        bool: Whether the neuron fired
        float: Time to action potential in milliseconds
        """
        if self.resting_potential + stimulus_strength * 1e-3 >= self.action_potential_threshold:
            # Calculate delay based on stimulus strength
            delay = 5 / (1 + stimulus_strength / 10)  # Simple model for delay in ms
            return True, delay
        else:
            return False, 0



class BipolarNeuron(Neuron):
    """
    Class for bipolar neurons, typically found in sensory systems like the retina.
    Bipolar neurons usually have two distinct processes (one dendritic and one axonal).
    """
    
    def __init__(self, neuron_type="bipolar"):
        self.neuron_type = neuron_type
        # Assume excitatory for sensory relay
        self.is_inhibitory = False
        self.is_excitatory = True
        
        # Soma characteristics (often slightly smaller)
        self.soma_diameter = 15e-6  # meters
        self.soma_volume = 2.0e-15  # cubic meters (approx.)
        self.membrane_thickness = 5e-9  # meters
        
        # Dendrite characteristics: one primary dendrite
        self.dendrite_count = 1
        self.dendrite_length = 200e-6  # meters
        self.dendrite_diameter = 1e-6  # meters
        self.dendritic_spine_density = 1.0  # spines per μm
        self.total_spine_count = int(self.dendritic_spine_density * self.dendrite_length * 1e6)
        
        # Axon characteristics
        self.axon_length = 0.05  # meters
        self.axon_diameter = 0.8e-6  # meters
        self.axon_initial_segment_length = 20e-6  # meters
        self.axon_collateral_count = 10  # fewer branches
        self.synapse_count = 2000
        
        # Myelin characteristics
        self.is_myelinated = True
        self.myelin_thickness = 0.15e-6
        self.internode_length = 80e-6
        self.node_of_ranvier_length = 1e-6
        
        # Electrical properties
        self.resting_potential = -65e-3  # volts
        self.action_potential_threshold = -50e-3  # volts
        self.action_potential_amplitude = 90e-3  # volts
        self.action_potential_duration = 1e-3  # seconds
        self.membrane_capacitance = 1e-2
        self.membrane_resistance = 1e8
        self.axial_resistance = 1.5
        
        # Neurotransmitter properties
        self.neurotransmitter = "glutamate"
        self.neurotransmitter_vesicle_count = 150000
        self.vesicle_diameter = 40e-9
        
        # Ion channels and receptors
        self.sodium_channel_density = 50
        self.potassium_channel_density = 15
        self.calcium_channel_density = 8
        
        # Metabolic properties
        self.mitochondria_count = 800
        self.glucose_consumption_rate = 4e-14
        self.atp_production_rate = 4e-13

        # inherit describe method from parent
        super().describe()
    
    def calculate_conduction_velocity(self):
        if self.is_myelinated:
            return 6 * self.axon_diameter * 1e6
        else:
            return 1.5
    
    def fire_action_potential(self, stimulus_strength):
        if self.resting_potential + stimulus_strength * 1e-3 >= self.action_potential_threshold:
            delay = 5 / (1 + stimulus_strength / 10)
            return True, delay
        else:
            return False, 0
