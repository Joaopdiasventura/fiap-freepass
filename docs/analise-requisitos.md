# Análise acadêmica, referências de engenharia e aderência

## 1. Documento normativo

O documento fornecido, **Global Solution 2026.2 — Prova substitutiva — AICSS**, determina **atividade individual** entregue no Portal do Aluno. O objetivo é: fenômeno observado → ESP32 com sensores e atuadores → alerta local. Exige dois sensores relevantes, um atuador, três níveis, medição periódica justificada, saída Serial com nível antes das leituras, evidências de diferentes entradas e identificação de limitações. A entrega deve ter capa com título/nome/RM, problema/público/evidência de relevância, arquitetura/origem dos dados, testes, código e pelo menos um ODS.

O website geral FIAP CitizenTech amplia a motivação social, mas os requisitos técnicos a cumprir nesta atividade são os do documento da disciplina. **IA executada no microcontrolador, transmissão de dados, cloud, GPS, armazenamento e aplicativo web não são exigências neste PDF.**

## 2. Critérios de avaliação

| Critério oficial                           | Pontos | Como o projeto procura atender                                                                     | Estado                                                           |
| ------------------------------------------ | -----: | -------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------- |
| Problema e contribuição CitizenTech        |      1 | Apoio à percepção de obstáculos em deslocamentos de pessoas com baixa visão; contexto OMS + ODS 11 | Texto implementado; relevância prática não validada com usuários |
| Coleta e decisão com sensores e atuadores  |      3 | HC-SR04 e LDR → `classify` → buzzer e 2 LEDs                                                       | Código implementado; integração host testada; Wokwi pendente     |
| Código alinhado ao observado na disciplina |      3 | ESP32 Arduino/C++, `setup/loop`, GPIO, `pulseIn`, ADC, `millis`, Serial                            | Implementado; compilação ESP32 pendente                          |
| Justificativa dos sensores e alinhamento   |      2 | Distância identifica aproximação; luminosidade ajusta antecipação                                  | Documentado e testado em política                                |
| Inovação apresentada                       |      1 | Limiar de atenção contextual, proporcional à luminosidade; alerta independente de conectividade    | Regra implementada; impacto não validado                         |

**Não é possível atribuir nota ou declarar atendimento integral demonstrado sem executar e registrar o Wokwi.**

## 3. Comparação com projetos anteriores

Inspeção dos arquivos da branch `main` dos repositórios indicados pelo solicitante, em 25/11/2026.

| Repositório           | O que foi observado no código                                                                                                            | Padrão aproveitado / ressalva                                                                                                                                                  |
| --------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `fleet-force-sensors` | `platformio.ini`, `diagram.json`, `wokwi.toml`, `src/main.ino`; leitura MPU6050 e potenciômetros; ciclo de 500 ms; JSON/HTTPS a cada 5 s | Constantes explícitas, leituras periódicas, responsabilidade local de aquisição; **não** reutilizar conectividade/telemetria sem necessidade                                   |
| `fiap-aicss-cp-02`    | NetGuard, entrada Serial, botão com debounce por `millis()`, validação de IPv4 e LEDs; limites declarados                                | Separação de funções, tratamento de entrada e estados; atenção à transparência sobre aquilo que não é implementado                                                             |
| `fiap-aicss-cp-1`     | Duas máquinas, eventos `SUCESSO/FALHA`, JSON com `timestamp_ms`, timers por `millis()`                                                   | Firmware enxuto, eventos auditáveis, intervalos explícitos                                                                                                                     |
| `fiap-memory-game`    | Vetores de botões/LEDs, sequenciamento de jogo e `delay()` para interação                                                                | Estrutura simples e sem dependências; **não** replicar bloqueios longos em uma rotina que precisa alertar rapidamente                                                          |
| `tars-sensors`        | DHT22, ADC para saída preditiva simulada, 3 LEDs, GPS opcional, SD/CSV, parâmetros em namespace, `TarsStateTracker.h`, testes host       | Testabilidade de lógica independente de hardware, estados nomeados, documentação de limitações; **não** reaproveitar previsão econômica, persistência ou sensor simulado de IA |

Repositórios: https://github.com/Joaopdiasventura/fleet-force-sensors ; https://github.com/Joaopdiasventura/fiap-aicss-cp-02 ; https://github.com/Joaopdiasventura/fiap-aicss-cp-1 ; https://github.com/Joaopdiasventura/fiap-memory-game ; https://github.com/Joaopdiasventura/tars-sensors .

### Padrão de desenvolvimento identificado

1. Arduino/C++ sobre ESP32, normalmente gerenciado pelo PlatformIO e executado com Wokwi.
2. Entradas, saídas, limites e intervalos em constantes perto do início do firmware.
3. Ciclos de `setup()`/`loop()` com funções pequenas e relatórios Serial legíveis ou estruturados.
4. Uso de `millis()` para periodicidade e máquinas de estados nos projetos mais recentes.
5. Descrição explícita de o que é simulado, especialmente GPS/potenciômetro/saída de IA.
6. Maior complexidade (rede, SD, GPS, CSV) apenas quando há necessidade do domínio.
7. Lógica pura com testes host no projeto TARS; adotada aqui para regras de alerta.

**Os projetos não são inteiramente homogêneos:** `fiap-memory-game` usa `delay()` e tem código concentrado; `tars-sensors` tem responsabilidades adicionais exigidas por seu próprio problema. A escolha para FreePass é uma síntese proporcional à prova, não uma cópia integral de um deles.

## 4. Por que é uma implementação diferente do TARS

| Dimensão                 | TARS original                                                  | FreePass novo                                                                  |
| ------------------------ | -------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| Problema                 | Antecipação de aumentos no preço de alimentos                  | Percepção complementar de obstáculos no deslocamento                           |
| Público                  | Famílias com restrição orçamentária                            | Pessoas com baixa visão                                                        |
| Entradas                 | Saída de previsão simulada por ADC + DHT22                     | Distância ultrassônica real/simulada + LDR                                     |
| Decisão                  | Conjugação de previsão de alta com temperatura/umidade         | Proximidade com limiares ajustados pela luminosidade                           |
| Comportamento            | Três LEDs estáticos; armazenamento de mudança de classificação | Dois LEDs e buzzer com frequências/cadências distintas, alerta local frequente |
| Evidência dos sensores   | Potenciômetro simula resultado de modelo; DHT22 mede ambiente  | Ambos medem grandezas diretamente relacionadas à regra proposta                |
| Persistência/comunicação | microSD, GPS opcional, CSV; Serial                             | Apenas Serial e atuadores locais                                               |
| Complexidade             | Telemetria e eventos de previsão/persistência                  | Leitura periódica, política pura e atuação sonora não bloqueante               |

## 5. Responsabilidades acadêmicas ainda pendentes

- Preencher nome e RM reais na capa do `RELATORIO_ENTREGA.md`.
- Compilar no PlatformIO para ESP32, executar Wokwi e capturar telas/Serial dos três níveis.
- Confirmar leituras com controles de distância e luminosidade do simulador.
- Reproduzir testes com o aluno responsável e documentar dificuldades, se houver.
- Anexar **evidências reais**, sem criar prints ilustrativos como prova de execução.

## 6. Fontes

- FIAP CitizenTech: https://experience.fiap.com.br/global-solution-citizen-tech
- Wokwi (componentes suportados): https://docs.wokwi.com/getting-started/supported-hardware
- Wokwi HC-SR04: https://docs.wokwi.com/parts/wokwi-hc-sr04
- Wokwi LDR: https://docs.wokwi.com/parts/wokwi-photoresistor-sensor
- Wokwi buzzer: https://docs.wokwi.com/parts/wokwi-buzzer
- OMS: https://www.who.int/news-room/fact-sheets/detail/blindness-and-visual-impairment/
- ONU — ODS 11: https://brasil.un.org/pt-br/sdgs/11
