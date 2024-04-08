# CORE23
# TODO
- [ ] Comb. di tasi per fare reset sulla Pilot, resetta sia Pilot che Core:
    - __disable_irq()
    - NVIC_SystemReset()

- [ ] Programmazione tasks
    - [X] CanHandlerTask
        - [X] Check ID.
        - [ ] Spostare cutoff nel gearTask.
        - [ ] Finire handling AS.
    - [X] PedalTask
    - [X] ADCTask
    - [X] TelemetryTask
    - [X] CheckModeTask
        - [X] Add signal to resume ASBCheckTask
        - [ ] Autonomous: se Una marcia è inserita, non deve poter permette di andare avanti -> gestire anche Neutral LED. (Neutrale led acceso)
    - [ ] ASBCheckTask
        - [ ] Vedere se serve capire se PC è acceso e può ricevere msg
    - [X] ErrHandASTask
    - [X] ASStateHandlerTask
        - [X] Finire PWM
    - [ ] GearTask -> solo manuale
        - [ ] Eliminare controllo sulla marcia, lo farà la Pilot23
    - [ ] AccASTask
        - [X] Aggiungere velocità a CanHandler
    
    - [ ] Gestire Brake Light sia in manual che autonomous.

    - [X] Sistemare priorità dei tasks
    - [ ] Gestire ridondanza sulla cremagliera, dare errore in caso di troppo drift tra i due sensori.