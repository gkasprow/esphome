import esphome.codegen as cg

energy_sampler_ns = cg.esphome_ns.namespace("energy_sampler")

EnergySampler = energy_sampler_ns.class_("EnergySampler")
Phase = energy_sampler_ns.enum("Phase", is_class=True)
PhaseSampleData = energy_sampler_ns.struct("PhaseSampleData")
EnergySampleData = energy_sampler_ns.struct("EnergySampleData")
