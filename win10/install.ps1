Set-ExecutionPolicy Bypass -Scope LocalMachine -Force

Remove-ItemProperty HKLM:\System\CurrentControlSet\Services\Rasman\Parameters -name "NegotiateDH2048_AES256"

Remove-ItemProperty HKLM:\System\CurrentControlSet\Services\PolicyAgent -name "AssumeUDPEncapsulationContextOnSendRule"

New-ItemProperty HKLM:\System\CurrentControlSet\Services\Rasman\Parameters -name "NegotiateDH2048_AES256" -value 1 -propertyType dword

New-ItemProperty HKLM:\System\CurrentControlSet\Services\PolicyAgent -name "AssumeUDPEncapsulationContextOnSendRule" -value 2 -propertyType dword
