from EventStore import EventStore


events = EventStore(["single_particle_etaRange_ptMin100000_ptMax100000_.root"])
for store in events:
    hits = store.get("positionedHits")
    num_prim = 0
    num_sec = 0
    for h in hits:
         if h.bits() == 1:
            num_prim += 1
         else:
            num_sec += 1
    print num_prim, num_sec

