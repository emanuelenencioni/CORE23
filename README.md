# CORE23
# TODO
- [ ] Comb. di tasi per fare reset sulla Pilot, resetta sia Pilot che Core:
    - __disable_irq()
    - NVIC_SystemReset()

- [ ] Programmazione tasks
    - [ ] CanHandlerTask
        - [ ] Check ID.
        - [ ] Spostare cutoff nel gearTask.
    - [ ] PedalTask
    - [ ] TelemetryTask
    - [ ] CheckModeTask
    - [ ] ASBCheckTask
    - [ ] ASStateHandlerTask