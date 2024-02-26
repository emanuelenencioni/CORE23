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
        - [ ] inviare nella ASCan tutti i dati per la pilot, devo capire meglio se sta cosa ha senso
    - [ ] PedalTask
    - [X] ADCTask
    - [ ] TelemetryTask
    - [X] CheckModeTask
        - [X] Add signal to resume ASBCheckTask
    - [X] ASBCheckTask
    - [X] ErrHandASTask
    - [X] ASStateHandlerTask
        -[X] Finire PWM
    - [ ] GearTask -> solo manuale
        - [ ] Eliminare controllo sulla marcia, lo farà la Pilot23
    - [ ] AccASTask
        - [ ] Aggiungere velocità a CanHandler