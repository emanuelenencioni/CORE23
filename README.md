# CORE23
# TODO
- [ ] Comb. di tasi per fare reset sulla Pilot, resetta sia Pilot che Core:
    - __disable_irq()
    - NVIC_SystemReset()

- [ ] Programmazione tasks
    - [ ] CanHandlerTask
        - [ ] Check ID.
        - [ ] Spostare cutoff nel gearTask.
        - [ ] Finire handling AS.
    - [ ] PedalTask
    - [ ] TelemetryTask
    - [ ] CheckModeTask
    - [ ] ASBCheckTask
    - [ ] ASStateHandlerTask
    - [ ] GearTask
        - [ ] Eliminare controllo sulla marcia, lo farà la Pilot23