# Task Set del Firenze Race
Di seguito esposti i vari task che la CORE23 dovrà eseguire:
### Controllo acceleratore (Prio 2)
- **Guida manuale**
Un task in grado di leggere l'ADC via DMA, per poi inoltrare gli stessi valori al motore. All'acceleratore abbiamo due sensori analogici con 2 funzioni di trasferimento diverse. Lo stesso comportamento va inoltrato alla centralina motore.
- **Guida autonoma**
In questo caso il task deve disabilitare la lettura dall'ADC e deve invece prendere i valori di accelerazione dal CAN bus, ricevuti dal PC.

### Invio telemetria (Prio 5)
- Telemetria del sistema autonomo e velocità del veicolo inviati sia alla EVO, collegata in CAN, sia al modulo LORA. La LoRa quando siamo in modalità autonomous riceverà anche i dati di telemetria che vengono inviati al data logger, quindi gli indirizzi 500, 501 502

### Controllo Ventole (Prio 7)
Inizializzazione PWM TIM_CHANNEL_3, TIM_CHANNEL_4. 
Da sensori di temperatora:
- MTS, sensore di temperatura dell'intercooler.
- WTS, Water Temperature Sensor. 

Una volta letti i valori dovranno essere mappate le corrispondenti velocità delle ventole.

### Gestione nodo CAN -> no task 
**Ricezione messaggi tramite interruzione, invio come risorsa condivisa**. 
- [ ] Aggiungere Semaforo alla FIFO del tx.
- Gestito con interruzione molto probabilmente, quindi sentire la prof.
2 Linee CAN da gestire:
- CAN1 (hcan1): Interfaccia CAN con il motore. Da qui si ottengono dati di telemetria tra cui la sensoristica.
- CAN2 (hcan2): Interfaccia CAN con PC e PILOT23. Dalla scheda PILOT23 si ottengono dati sul cambio marcia, e occorre inviare i dati del motore (giri, temperature etc). Con l'Autonomous System occorre inviare lo stato dell'EBS (Emergency Brake System)per poi dare lo stato del sistema autonomo della competizione. Ci saranno poi dati in ricezione sull'acceleratore o eventuali errori del PC. quest'uiltimo deve inoltre inviare ogni tot. milissecondi un messaggio di "Alive" per settare una variabile booleana. Questo settaggio permette di controllare se gli algoritmi del PC sono ncora online. Inoltre si dovrà aggiornare una variabile booleana per ogni CAN per capire se i bus stanno funzionando correttamente.



### Gestione errori Manual mode(Prio 1)
Ad esempio: Pressioni motore, temperature, fuori range.

 
### Cambio marcia (Prio 4)
Un task che se richiesto via CAN  dalla PILOT23 o dal sistema autonomo, occorre aumentare o ridurre una marcia.  Gestire anche il CUTOFF. Gestione anche frizione. alla partenza. Cambio marcia: Semplificato, praticamente non metteremo limiti di gear per la guida manuale in caso di fallimento del desmo. Lo faremo invece per la guida autonoma.
- [ ] Fare ridondanza: se non funziona gear_prox, usare attesa minima. Quindi semplicemente implementare timeout di 1 secondo ad esempio.
- [ ] Sentire Sandro range di cut-off.

### Gestione stato del sistema autonomo (Prio 5)
Implementare Flow Chart del regolamento per poi inviare i dati sullo stato al PC, gestire Autonomous System Status Indicator e la sirena.

#### Gestione EBS e altri flowchart dell'ASF = error Handler(Prio 2)
Durante una prima esecuzione, all'avvio di tutta la parte autonoma del sistema, dovrà essere verificato il corretto funzionamento del EBS. Per fare questo occorre attivare e disattivare le valvole per mandare il sistema frenante in pressione, dopodiché capire se appunto il sistema frenerà per poi dare l'ok all'Autonomous System. Errori dell'AS, errori attuatori (sterzo)

TODO Capire l'encoder dello sterzo come funziona per fare la ridondanza.

Implementazione di un watchdog  manuale, che permetta di chiudere lo Shutdown Cirtcuti in caso il sistema autonomo, le linee CAN, il RES (Remote Emergency System),  siano offline per abbastaza tempo (circa 200 ms), o se le bombole di aria compressa dell'EBS  scendono sotto una certa pressione soglia.
Ridondanza per sensore cremagliera -> lettura da telemetria.

### One shot task: task dell'initial EBS
Task che una volta eseguito l'initial setup, va a rilasciare i task dell'error handler dell'AS e il task della gestione, attiva quindi a cascata tutti i task relativi all'autonomous System. 
Task attivato al momento della chiusura dell'ASMS. guardare specifico GPIO. controllare già missione selezionata e in modalità autonomous da Pilot.

Viene attivato dal main della core



#### **Check Mode: Periodic Task**
Controlla la scelta della modalità di funzionamento dalla pilot. Rilascia il taskReleasing. Gestire la possibilità di cambio modalità in caso di cambio real time della missione/mode finché non è accesa l'asms e il motore non è acceso. 
ASMS attivo solo dopo aver selezionato la missione e confermata, altrimenti da errore.

- Chiedere alla prof: Periodo/Deadline/Priorità dinamica nelle ptpn sono un problema?



Procedura Auto autonomo:
- Accendere pc
- LVMS on
- Pilot23: selezionare missione da selettore di destra, premere quindi ok.
- ASMS on
-  in 2 - 4 s accendere la macchina ( controllare sia in folle)
- se EBS ok -> 


| Capabilities| Description | Collaboration |
| ----------- | ----------- | ------------- |
| Init      | Hw and Sw initialization       ||
| PILOT23_comm   | Communication with Pilot23 |PILOT23|
| PC_comm | Communication with the Autonomous PC | Autonomous PC|
| engine_comm   | Communication with Engine ECU (Mectronik MKE7) |MKE7|
| TU_Comm   | Communication with AIM EVO 5 | TU|
| ADC_read   | Reading all the sensor data from ADCs | all Sensors|
| DAC_write  | Writing data on the DAC ||
| Auto_Cmd  | Management of the Autonomous System Status ||









