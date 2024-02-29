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
- [x] Sentire Sandro range di cut-off. -
Il cutoff si può attivare a qualsiasi range di giri del motore, però per evitare di stressarlo, conviene attivarlo sopra un certo numero di giri (penso 6000 come aveva messo Naldo).

### Gestione stato del sistema autonomo (Prio 5)
Implementare Flow Chart del regolamento per poi inviare i dati sullo stato al PC, gestire Autonomous System Status Indicator e la sirena.

#### Gestione EBS e altri flowchart dell'ASF = error Handler(Prio 2)
Durante una prima esecuzione, all'avvio di tutta la parte autonoma del sistema, dovrà essere verificato il corretto funzionamento del EBS. Per fare questo occorre attivare e disattivare le valvole per mandare il sistema frenante in pressione, dopodiché capire se appunto il sistema frenerà per poi dare l'ok all'Autonomous System. Errori dell'AS, errori attuatori (sterzo)

- [ ] Capire l'encoder dello sterzo come funziona per fare la ridondanza.

Implementazione di un watchdog  manuale, che permetta di chiudere lo Shutdown Cirtcuti in caso il sistema autonomo, le linee CAN, il RES (Remote Emergency System),  siano offline per abbastaza tempo (circa 200 ms), o se le bombole di aria compressa dell'EBS  scendono sotto una certa pressione soglia.
Ridondanza per sensore cremagliera -> lettura da telemetria.

### One shot task: task dell'initial EBS
Task che una volta eseguito l'initial setup, va a rilasciare i task dell'error handler dell'AS e il task della gestione, attiva quindi a cascata tutti i task relativi all'autonomous System. 
Task attivato al momento della chiusura dell'ASMS. guardare specifico GPIO. controllare già missione selezionata e in modalità autonomous da Pilot.
Per disattivare l'errore sviluppato da questo task, occorre aprire LVMS.


#### AS ErrorHandler
Ad esempio dal nodo CAN viene richiamato ErrorHandler() che va a scrivere su una variabile booleana. Questa viene checkata da questo nodo. Se è true allora si da errore.
- [ ] Vedere se aggiungere un check della can per eventuali messaggi di errore del driver del freno. Forse meglio farlo fare direttamente al PC e poi lui inoltrerà. 

#### **Check Mode: Periodic Task**
Controlla la scelta della modalità di funzionamento dalla pilot. Rilascia il taskReleasing. Gestire la possibilità di cambio modalità in caso di cambio real time della missione/mode finché non è accesa l'asms e il motore non è acceso. 
ASMS attivo solo dopo aver selezionato la missione e confermata, altrimenti da errore.
Essendo un task periodico, c'è da prendere in considerazione che verrà eseguito più volte. Perciò le attivazioni dei vari task non si possono fare più volte, ma una sola. altrimenti non funziona nulla.


#### AS State Handler
Attivato subito, controlla le varie variabili condivise degli altri task.


### Varie
- Manual mode in automatico senza selezionare nulla.
- **Gestione task attivati da altri task**
	Per i task attivati da altri, saranno istanziati dallo scheduler di freeRTOS, poi saranno subito sospesi e riattivati solo dalle specifiche condizioni dai relativi task, come si può vedere nell'Activity Diagram.

- **vTaskDelayUntil** -> per i task periodici

- **Procedura Auto autonomo**:
	
	- LVMS on
	- Accendere pc
	- Pilot23: selezionare missione da selettore di destra, premere quindi ok.
	- ASMS on
	-  in 2 - 4 s accendere la macchina ( controllare sia in folle)
	- se EBS ok -> invio msg a PC "Start" per finire di avviare tutti i nodi e mettere in pressione l'ASB.
	- [AS Ready]: Aspetta il go dal RES
	- il go letto da StateHandler, setta prima ASSI a Driving, e lo stato a driving, e invia il segnale Go anche al PC.
	- Se ASEMergency o finish, apposto.
	- Tornare alla macchina e spegnere tutti i MS.



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

### Dati temporali
Presi facendo una media di 10000 takes. Singolarmente su ogni task.
$2^{32} - 1 =  4294967296$




### Domande prof
- Le funzioni di entry point, lei, guardando le slide ne usava come una di inizio e una di fine, però non dovrebbe essere una sola? Nel nostro caso, ogni task esegue una funzione "xxThread" che ha il ciclo infinito nel caso dei task periodici etc...  Come dobbiamo fare quindi? 
- metterne tutti se non sono tanti.
- 
- Uso dei semafori, chiedere nel nostro caso essendo single core... serve anche in lettura? forse no. Sentire la prof per la pre-emption. Se servo o no comunque a seconda dei tempi di esecuzione.
- no 
- Chiedere in generale se la doc è corretta per continuare così.
- astrazione +, analisi WCET
- Le entry point, abbiamo visto ad esempio nel modello che metteva nelle slide,metteva le entry points di inizio e fine con relativi semafori. noi dobbiamo fare uguale?
CAnHandler task:
 - Gestire la CAN: stm32 usa interrupt, forse meglio usare un task che legge e scrive sulle varie "code" condivise tra task?, il primo modo sarebbe più sicuro sulla scrittura. Però fare l'interrupt per ogni messaggio può essere costoso?
Check mode task
	- Chiedere alla prof: Periodo/Deadline/Priorità dinamica nelle ptpn sono un problema?
- 
lettura dati: tutto prima: dipende, da quanto tempo impiega la lettura. attenzione, magari si usa una versione del dato non aggiornata.
