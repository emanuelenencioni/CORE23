# Task Set del Firenze Reis
Di seguito esposti i vari task che la CORE23 dovrà eseguire:
### Controllo acceleratore (Prio 2)
- **Guida manuale**
Un task in grado di leggere l'ADC via DMA, per poi inoltrare gli stessi valori al motore. All'acceleratore abbiamo due sensori analogici con 2 funzioni di trasferimento diverse. Lo stesso comportamento va inoltrato alla centralina motore.
- **Guida autonoma**
In questo caso il task deve disabilitare la lettura dall'ADC e deve invece prendere i valori di accelerazione dal CAN bus, ricevuti dal PC.

### Invio telemetria (Prio 5)
- Telemetria del sistema autonomo e velocità del veicolo inviati sia alla EVO, collegata in CAN, sia al modulo LORA.

### Controllo Ventole (Prio 7)
Inizializzazione PWM TIM_CHANNEL_3, TIM_CHANNEL_4. 
Da sensori di temperatora:
- MTS, sensore di temperatura dell'intercooler.
- WTS, Water Temperature Sensor. 

Una volta letti i valori dovranno essere mappate le corrispondenti velocità delle ventole.

### Gestione nodo CAN (Prio 2)
- Gestito con interruzione molto probabilmente, quindi sentire la prof.
2 Linee CAN da gestire:
- CAN1 (hcan1): Interfaccia CAN con il motore. Da qui si ottengono dati di telemetria tra cui la sensoristica.
- CAN2 (hcan2): Interfaccia CAN con PC e PILOT23. Dalla scheda PILOT23 si ottengono dati sul cambio marcia, e occorre inviare i dati del motore (giri, temperature etc). Con l'Autonomous System occorre inviare lo stato dell'EBS (Emergency Brake System)per poi dare lo stato del sistema autonomo della competizione. Ci saranno poi dati in ricezione sull'acceleratore o eventuali errori del PC. quest'uiltimo deve inoltre inviare ogni tot. milissecondi un messaggio di "Alive" per settare una variabile booleana. Questo settaggio permette di controllare se gli algoritmi del PC sono ncora online. Inoltre si dovrà aggiornare una variabile booleana per ogni CAN per capire se i bus stanno funzionando correttamente.

### Gestione errori (Prio 1)
Durante una prima esecuzione, all'avvio di tutta la parte autonoma del sistema, dovrà essere verificato il corretto funzionamento del EBS. Per fare questo occorre attivare e disattivare le valvole per mandare il sistema frenante in pressione, dopodiché capire se appunto il sistema frenerà per poi dare l'ok all'Autonomous System.

Implementazione di un watchdog  manuale, che permetta di chiudere lo Shutdown Cirtcuti in caso il sistema autonomo, le linee CAN, il RES (Remote Emergency System),  siano offline per abbastaza tempo (circa 200 ms), o se le bombole di aria compressa dell'EBS  scendono sotto una certa pressione soglia.

### Cambio marcia (Prio 4)
Un task che se richiesto via CAN  dalla PILOT23 o dal sistema autonomo, occorre aumentare o ridurre una marcia.  Gestire anche il CUTOFF.

### Gestione stato del sistema autonomo (Prio 5)
Implementare Flow Chart del regolamento per poi inviare i dati sullo stato al PC, gestire Autonomous System Status Indicator e la sirena.



