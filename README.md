# Mission Control IA — Sistema IoT de Sensores da Cápsula Espacial

Sistema embarcado simulado no Tinkercad para monitoramento de uma cápsula espacial experimental, integrado à proposta do **Mission Control IA**.

O projeto utiliza Arduino Uno, sensores simulados, display LCD 16x2, LED RGB e buzzer para coletar, processar e exibir dados físicos da cápsula em tempo real.

---

## Informações do projeto

**Projeto:** Mission Control IA
**Missão:** Artemis Deep Scan
**Disciplina:** Computer Organization and Architecture
**Arquivo principal:** `sistema_sensores_mission_control.ino`

**Link do projeto no Tinkercad:** [INSERIR_LINK_DO_TINKERCAD]
**Link do vídeo de demonstração:** [INSERIR_LINK_DO_VIDEO]
**Relatório técnico:** `relatorio_tecnico_computer_organization.pdf`

---

## Integrantes

* Caio César Portela França — RM: 573127
* Gustavo Curis de Francisco — RM: 569704
* Tiago Pimentel Muniz — RM: 574148

---

## Visão geral

O sistema representa a camada embarcada de sensoriamento físico da cápsula espacial dentro do ecossistema **Mission Control IA**.

A proposta é simular a coleta local de dados por sensores conectados a um microcontrolador, permitindo que a cápsula monitore variáveis críticas da missão e apresente alertas locais em tempo real.

O circuito monitora:

* temperatura interna da cápsula;
* temperatura do módulo de energia;
* luminosidade externa/incidência solar;
* impacto estrutural;
* instabilidade mecânica nos eixos X e Y.

A partir dessas leituras, o Arduino classifica o estado geral da cápsula como:

* **NORMAL**
* **ATENÇÃO**
* **CRÍTICO**

---

## Objetivo

Desenvolver um sistema IoT para monitoramento de uma cápsula espacial simulada, utilizando sensores e atuadores no Tinkercad.

O sistema tem como objetivo:

* coletar dados físicos simulados;
* processar as leituras em tempo real;
* exibir informações no LCD 16x2;
* indicar o status operacional por LED RGB;
* emitir alerta sonoro em condição crítica;
* demonstrar conceitos de sistemas embarcados, sensores, atuadores e programação de microcontroladores.

---

## Componentes utilizados

| Componente          | Quantidade | Função                                                |
| ------------------- | ---------: | ----------------------------------------------------- |
| Arduino Uno R3      |          1 | Processamento das leituras e controle dos atuadores   |
| TMP36               |          2 | Medição da temperatura interna e do módulo de energia |
| LDR / Photoresistor |          1 | Simulação da luminosidade externa/incidência solar    |
| Force Sensor        |          1 | Simulação de impacto ou pressão estrutural            |
| Tilt Sensor         |          2 | Simulação de instabilidade mecânica nos eixos X e Y   |
| Display LCD 16x2    |          1 | Exibição contínua dos dados monitorados               |
| LED RGB             |          1 | Indicação visual do status operacional                |
| Buzzer / Piezo      |          1 | Alerta sonoro em condição crítica                     |
| Resistores          |   Diversos | Proteção elétrica e divisores de tensão               |
| Protoboard          |          1 | Organização das conexões do circuito                  |

---

## Mapa de pinos

| Pino Arduino | Componente   | Função                                |
| ------------ | ------------ | ------------------------------------- |
| A0           | TMP36        | Temperatura interna da cápsula        |
| A1           | TMP36        | Temperatura do módulo de energia      |
| A2           | LDR          | Luminosidade externa/incidência solar |
| A3           | Force Sensor | Impacto estrutural                    |
| D2           | Tilt Sensor  | Instabilidade no eixo X               |
| D3           | Tilt Sensor  | Instabilidade no eixo Y               |
| D4           | LED RGB      | Canal vermelho                        |
| D5           | LED RGB      | Canal verde                           |
| D6           | LED RGB      | Canal azul                            |
| D7           | Buzzer       | Alerta sonoro                         |
| D8           | LCD RS       | Controle do display                   |
| D9           | LCD E        | Controle do display                   |
| D10          | LCD D4       | Dados do display                      |
| D11          | LCD D5       | Dados do display                      |
| D12          | LCD D6       | Dados do display                      |
| D13          | LCD D7       | Dados do display                      |

---

## Lógica de funcionamento

O sistema executa leituras contínuas dos sensores e classifica o estado da cápsula com base em três grupos principais.

### Estado térmico

A temperatura é medida em dois pontos:

* temperatura interna da cápsula;
* temperatura do módulo de energia.

Se a temperatura ultrapassar os limites definidos no código, o sistema altera o estado térmico para **ATENÇÃO** ou **CRÍTICO**.

### Estado solar

O LDR mede a luminosidade externa, simulando a incidência solar sobre a cápsula.

Essa leitura representa a disponibilidade de luz externa, conectando o circuito ao contexto energético do Mission Control IA.

### Estado mecânico

A instabilidade mecânica é representada por uma leitura composta:

* o Force Sensor simula impacto ou pressão estrutural;
* os Tilt Sensors simulam inclinação nos eixos X e Y;
* eventos repetidos são interpretados como possível turbulência ou falha mecânica.

---

## Classificação operacional

| Status  | Condição geral                               | Saída visual/sonora       |
| ------- | -------------------------------------------- | ------------------------- |
| NORMAL  | Leituras dentro dos limites seguros          | LED RGB verde             |
| ATENÇÃO | Uma ou mais variáveis em faixa intermediária | LED RGB amarelo           |
| CRÍTICO | Alguma variável em condição crítica          | LED RGB vermelho + buzzer |

---

## Exibição dos dados

O sistema apresenta as leituras em dois meios.

### LCD 16x2

O LCD alterna telas com:

* temperatura interna;
* temperatura do módulo de energia;
* status geral;
* incidência solar;
* impacto estrutural;
* estado dos sensores Tilt;
* alerta principal.

### Monitor Serial

O Monitor Serial exibe a telemetria detalhada da cápsula, incluindo:

* temperatura interna;
* temperatura do módulo de energia;
* incidência solar;
* impacto estrutural;
* Tilt X;
* Tilt Y;
* status térmico;
* status solar;
* status mecânico;
* status geral da cápsula;
* alerta principal;
* eventos mecânicos detectados.

---

## Como executar

1. Acesse o projeto no Tinkercad:
   [INSERIR_LINK_DO_TINKERCAD]

2. Abra a simulação do circuito.

3. Verifique se o arquivo carregado é:

```text
sistema_sensores_mission_control.ino
```

4. Clique em **Start Simulation**.

5. Observe as leituras no LCD 16x2.

6. Abra o **Monitor Serial** em 9600 baud para visualizar a telemetria detalhada.

7. Altere os sensores na simulação para validar os estados **NORMAL**, **ATENÇÃO** e **CRÍTICO**.

---

## Código principal

O código está disponível no arquivo:

```text
sistema_sensores_mission_control.ino
```

Principais funções implementadas:

* leitura dos sensores analógicos;
* leitura dos sensores digitais;
* conversão do TMP36 para graus Celsius;
* conversão de sensores analógicos para percentual;
* classificação térmica;
* classificação solar;
* classificação mecânica;
* classificação geral da cápsula;
* exibição no LCD;
* impressão no Monitor Serial;
* acionamento do LED RGB;
* acionamento do buzzer em estado crítico.

---


## Tecnologias utilizadas

* Arduino Uno
* Tinkercad Circuits
* Linguagem C/C++ para Arduino
* Display LCD 16x2
* Sensores analógicos
* Sensores digitais
* LED RGB
* Buzzer
* Monitor Serial

---

## Relação com o Mission Control IA

Este circuito representa a origem física dos dados monitorados pelo **Mission Control IA**.

Enquanto o sistema principal atua como central de missão, com análise, interface e tomada de decisão, o circuito em Tinkercad simula a camada embarcada responsável por coletar dados diretamente da cápsula.

Assim, o projeto conecta:

* sensores físicos simulados;
* processamento embarcado;
* alertas locais;
* telemetria operacional;
* lógica de classificação de risco;
* narrativa integrada de missão espacial.

---

## Observação

O sistema desenvolvido é uma simulação acadêmica. Os dados obtidos no Tinkercad representam leituras simuladas de uma cápsula espacial experimental e foram estruturados para demonstrar conceitos de IoT, sistemas embarcados, sensores, atuadores e monitoramento em tempo real.
