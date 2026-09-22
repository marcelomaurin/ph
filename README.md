# Documentação técnica — Sistema de medição de pH

**Projeto:** `marcelomaurin/ph`  
**Plataforma:** Arduino Nano clássico (ATmega328P, 5 V)  
**Arquivo principal:** `ph/ph.ino`  
**Finalidade:** medir o pH de uma solução, acompanhar sua temperatura e apresentar os resultados em um display LCD.

## 1. Visão geral

O projeto implementa um instrumento eletrônico de bancada para monitoramento de pH. O sistema recebe o sinal de uma sonda de pH por meio de um módulo condicionador analógico, converte esse sinal com o conversor analógico-digital do Arduino, mede a temperatura com um termopar conectado a um MAX6675 e mostra pH e temperatura em um display LCD 16 × 2 com interface I²C.

O firmware também recebe comandos pela porta serial e grava um valor de calibração na memória EEPROM, permitindo conservar esse valor quando o equipamento é desligado. No estado atual do código, porém, o valor armazenado não é aplicado à fórmula final de pH. A fórmula existente também precisa ser corrigida antes que o protótipo seja utilizado como instrumento quantitativo.

## 2. Blocos funcionais

1. **Sonda de pH:** transforma a atividade dos íons hidrogênio em uma diferença de potencial elétrico.
2. **Módulo condicionador de pH:** apresenta alta impedância de entrada, amplifica e desloca o pequeno sinal da sonda para uma faixa aceita pelo Arduino.
3. **Arduino Nano:** realiza a conversão analógico-digital, filtra amostras, calcula o resultado e coordena os periféricos.
4. **MAX6675 e termopar tipo K:** medem a temperatura da solução ou de seu entorno.
5. **LCD 16 × 2 I²C:** apresenta o pH e a temperatura.
6. **EEPROM e porta serial:** permitem armazenar e configurar parâmetros de calibração.

> A sonda de pH não deve ser ligada diretamente ao pino A0. Eletrodos de vidro têm impedância muito alta e exigem um circuito condicionador próprio para pH.

## 3. Princípios físicos da medição de pH

### 3.1 Definição de pH

O pH representa, de forma logarítmica, a atividade dos íons hidrogênio de uma solução:

\[
\mathrm{pH}=-\log_{10}(a_{H^+})
\]

Em aplicações diluídas, a atividade é frequentemente aproximada pela concentração. A escala normalmente utilizada vai de 0 a 14: valores menores que 7 indicam meio ácido, 7 indica neutralidade aproximada e valores maiores que 7 indicam meio básico, considerando condições usuais.

### 3.2 Eletrodo de vidro

Uma sonda combinada de pH contém um eletrodo de vidro sensível aos íons H⁺ e um eletrodo de referência. A membrana hidratada do vidro desenvolve um potencial que depende da diferença de atividade de H⁺ entre a solução interna conhecida e a amostra. O eletrodo de referência fornece um potencial estável. O instrumento mede a diferença entre esses potenciais.

O sinal produzido é pequeno, normalmente da ordem de dezenas ou centenas de milivolts, e a fonte possui impedância muito elevada. Por isso, o condicionador deve ter corrente de polarização muito baixa e impedância de entrada elevada. Cabos longos, umidade, sujeira, conectores inadequados e ruído elétrico podem alterar sensivelmente a leitura.

### 3.3 Equação de Nernst

Para um eletrodo ideal, a relação entre potencial e pH pode ser expressa por:

\[
E=E_0-\frac{2{,}303RT}{nF}\,\mathrm{pH}
\]

onde:

- \(E\) é o potencial medido;
- \(E_0\) reúne o potencial no ponto de referência e os deslocamentos do sistema;
- \(R=8{,}314\;\mathrm{J\,mol^{-1}\,K^{-1}}\) é a constante dos gases;
- \(T\) é a temperatura absoluta em kelvin;
- \(n=1\) para o íon H⁺;
- \(F=96485\;\mathrm{C\,mol^{-1}}\) é a constante de Faraday.

A sensibilidade teórica é:

\[
S(T)=\frac{2{,}303RT}{F}
\]

A 25 °C, \(S\) é aproximadamente 59,16 mV por unidade de pH. A sensibilidade cresce com a temperatura. Na prática, deslocamento e inclinação devem ser determinados por calibração com soluções tampão.

### 3.4 Compensação de temperatura

A temperatura altera a inclinação eletroquímica do eletrodo. Ela também pode alterar o pH real da própria solução tampão ou amostra. A compensação pela equação de Nernst corrige a resposta do eletrodo; não transforma automaticamente o pH da amostra no valor que ela teria a 25 °C.

O termopar e a sonda devem estar em equilíbrio térmico com a mesma amostra. Para maior exatidão na faixa ambiente, um sensor como PT100/PT1000 ou DS18B20 devidamente calibrado pode ser mais apropriado que um termopar tipo K, embora o MAX6675 seja funcional para o protótipo.

## 4. Componentes necessários

- Arduino Nano clássico, baseado no ATmega328P, com ADC de 10 bits e lógica de 5 V;
- sonda combinada de pH com conector compatível;
- módulo condicionador/amplificador para sonda de pH, com saída analógica de 0 a 5 V;
- módulo MAX6675;
- termopar tipo K;
- display LCD 16 × 2 com interface I²C, normalmente no endereço `0x27`;
- fonte de alimentação estável;
- jumpers e protoboard para testes, ou placa de circuito para a versão definitiva;
- soluções tampão de pH 4,00, 7,00 e, conforme a faixa de trabalho, 10,00;
- água deionizada para enxágue e solução própria para armazenamento da sonda.

## 5. Ligações elétricas

As ligações abaixo foram deduzidas diretamente das definições presentes no firmware.

| Módulo | Sinal | Pino no Arduino | Observação |
|---|---|---:|---|
| Condicionador de pH | Saída analógica | A0 | Deve permanecer entre 0 e 5 V |
| Condicionador de pH | VCC | 5 V | Confirmar a tensão exigida pelo módulo |
| Condicionador de pH | GND | GND | Terra comum com o Arduino |
| MAX6675 | SO/DO | D12 | Dados seriais do conversor |
| MAX6675 | CS | D10 | Seleção do dispositivo |
| MAX6675 | SCK/CLK | D13 | Relógio serial |
| MAX6675 | VCC | 5 V ou 3,3 V | Conforme o módulo utilizado |
| MAX6675 | GND | GND | Terra comum |
| LCD I²C | SDA | A4 | Barramento I²C do Arduino Nano clássico |
| LCD I²C | SCL | A5 | Barramento I²C do Arduino Nano clássico |
| LCD I²C | VCC | 5 V | Verificar o backpack I²C |
| LCD I²C | GND | GND | Terra comum |

### Cuidados de montagem

- Desligue a alimentação antes de alterar qualquer ligação.
- Confirme a pinagem impressa no MAX6675 e no LCD; a ordem dos pinos varia entre fabricantes.
- Não exponha A0 a tensão negativa nem superior à referência do ADC.
- Mantenha o cabo da sonda afastado de relés, motores, fontes chaveadas e condutores de corrente elevada.
- Use conexão BNC e blindagem adequadas para a sonda.
- Evite protoboard suja ou úmida na entrada de alta impedância.
- Em uma placa definitiva, separe a seção analógica da digital e use desacoplamento de 100 nF próximo aos módulos.
- Se a solução puder estar eletricamente ligada a bombas, resistências ou outros equipamentos, avalie isolamento galvânico.
- Esta pinagem considera o Arduino Nano clássico com ATmega328P. Arduino Nano Every, Nano 33 e outras variantes podem usar outra arquitetura, tensão lógica ou referência analógica e devem ser conferidos separadamente.

## 6. Montagem passo a passo

1. **Monte a alimentação e o terra comum.** Ligue Arduino, LCD, MAX6675 e condicionador conforme as tensões especificadas por seus fabricantes.
2. **Ligue o LCD.** Conecte SDA e SCL aos pinos I²C da placa. O código utiliza endereço `0x27`; se nada aparecer, execute um scanner I²C e ajuste o endereço.
3. **Ligue o conjunto de temperatura.** Conecte SO a D12, CS a D10 e CLK a D13. Instale o termopar no borne do MAX6675, respeitando a polaridade.
4. **Ligue o módulo de pH.** Conecte somente a saída condicionada do módulo ao A0. Encaixe a sonda no conector BNC do condicionador.
5. **Inspecione a montagem.** Confirme ausência de curto-circuitos, polaridades, tensão de alimentação e terra comum.
6. **Carregue o firmware.** Instale as bibliotecas `LiquidCrystal_I2C` e `max6675`, selecione placa e porta e grave `ph/ph.ino`.
7. **Abra o monitor serial.** Configure 115200 bit/s e final de linha como nova linha.
8. **Teste separadamente.** Verifique primeiro o LCD, depois a temperatura e finalmente a tensão da saída do módulo de pH.
9. **Calibre com tampões.** Aguarde estabilização, enxágue a sonda entre soluções e determine deslocamento e inclinação.
10. **Valide.** Meça novamente os tampões e uma solução de verificação independente.

## 7. Como o firmware foi desenvolvido

O programa foi organizado em funções específicas para inicialização, aquisição e apresentação:

- `Start_Serial()` abre a comunicação a 115200 bit/s;
- `Start_Ambiente()` limpa o buffer e chama a leitura da EEPROM;
- `Start_PH()` configura A0 como entrada;
- `Start_LCD()` inicializa o LCD `0x27`, mostra a tela de abertura e ativa a iluminação;
- `Le_PH()` coleta dez amostras analógicas, converte cada leitura para tensão, calcula uma média e tenta converter o resultado em pH;
- `Le_Temperatura()` obtém a temperatura do MAX6675;
- `Le_Serial()` interpreta comandos com o formato `calibrar:<valor>`;
- `Display()` apresenta pH e temperatura;
- `loop()` repete aquisição, comando e exibição aproximadamente uma vez por segundo.

O ADC é tratado como conversor de 10 bits:

\[
V=\frac{ADC\times5{,}0}{1024}
\]

Essa expressão pressupõe alimentação/referência de exatamente 5,0 V. Em um Arduino de 10 bits, os códigos variam de 0 a 1023; por isso, para uma conversão baseada no valor de fundo de escala, costuma-se usar 1023. A melhor solução é medir a referência real ou utilizar uma referência estável e caracterizada.

O código usa ainda uma suavização recursiva:

```text
media_analogica = (nova_amostra + media_analogica) / 2
```

Como `media_analogica` não é reiniciada no início de cada ciclo, ela funciona como um filtro exponencial que mantém memória entre as leituras. Depois, as dez tensões filtradas são novamente promediadas.

## 8. Calibração correta

### 8.1 Procedimento recomendado de dois pontos

1. Ligue o equipamento e aguarde estabilização térmica.
2. Hidrate a sonda conforme a recomendação do fabricante. Nunca armazene permanentemente o bulbo em água deionizada.
3. Enxágue com água deionizada e remova o excesso sem esfregar o bulbo.
4. Mergulhe a sonda e o sensor de temperatura no tampão pH 7,00.
5. Aguarde estabilidade e registre a tensão média \(V_7\) e a temperatura.
6. Enxágue e repita no tampão pH 4,00, obtendo \(V_4\). Para medições alcalinas, pode-se usar pH 10,00.
7. Calcule a inclinação experimental:

\[
m=\frac{V_4-V_7}{4-7}
\]

8. Para uma tensão medida \(V\), calcule:

\[
\mathrm{pH}=7+\frac{V-V_7}{m}
\]

O sinal de \(m\) depende do condicionador. Não se deve presumir o sentido sem medir os tampões. Uma calibração de três pontos permite avaliar não linearidade e melhorar a faixa ácida/alcalina.

### 8.2 Parâmetros a armazenar

Para uma calibração reproduzível, a EEPROM deve guardar pelo menos:

- tensão ou código ADC no tampão pH 7,00;
- inclinação em volts por pH ou dois pontos completos;
- temperatura de calibração;
- marcador de validade, versão e, idealmente, verificação de integridade.

O comando serial atual, `calibrar:<valor>`, salva apenas um número. Após a gravação, o firmware também deve atualizar a variável em RAM ou reler a EEPROM.

## 9. Limitações e correções necessárias no código atual

### 9.1 Fórmula de Nernst incompleta

`calcularInclinacao()` retorna `R*T/(F*N)`, mas a inclinação por unidade de pH requer o fator `2,303`. O valor atual corresponde a aproximadamente 25,7 mV a 25 °C, e não aos 59,16 mV/pH esperados.

### 9.2 Incompatibilidade dimensional

A expressão atual é:

```cpp
valor_pH = (media - 7) / inclinacao;
```

Ela subtrai o número de pH 7 de uma tensão em volts. Essas grandezas não são compatíveis. Deve-se subtrair a tensão de referência obtida no tampão pH 7 e então dividir pela inclinação calibrada, acrescentando o pH de referência.

### 9.3 Calibração não utilizada

`valor_calibracao` é lido e gravado na EEPROM, porém não participa do cálculo de `valor_pH`. Além disso, após `Grava_ValorCalibracao(valor)`, a variável em RAM não recebe o novo valor.

### 9.4 Ordem de aquisição

No `loop()`, `Le_PH()` é executada antes de `Le_Temperatura()`. Assim, o cálculo utiliza a temperatura do ciclo anterior; no primeiro ciclo, utiliza o valor global inicial, zero. A temperatura deve ser lida e validada antes do pH.

### 9.5 EEPROM não inicializada

Uma EEPROM nova pode conter bytes que não representam um `float` válido. É necessário validar faixa, finitude e assinatura antes de aceitar o parâmetro; se inválido, deve-se carregar um padrão seguro e exigir calibração.

### 9.6 Tratamento de falhas

O código não detecta termopar aberto, leitura `NaN`, saída analógica fora da faixa, instabilidade excessiva ou desconexão da sonda. Esses estados devem aparecer no LCD e na serial, sem apresentar um pH aparentemente válido.

### 9.7 Display

Quando um novo valor tem menos caracteres que o anterior, podem permanecer dígitos antigos no LCD. Recomenda-se preencher o campo com espaços antes de imprimir ou usar uma string de largura fixa.

## 10. Sequência de funcionamento esperada

1. O sistema inicia a serial, lê e valida os parâmetros de calibração, configura os sensores e inicializa o LCD.
2. O MAX6675 mede a temperatura da amostra.
3. O Arduino realiza várias leituras da saída condicionada da sonda.
4. As amostras são filtradas e convertidas usando a referência real do ADC.
5. O firmware aplica os coeficientes obtidos na calibração e, se adotado, ajusta a inclinação pela temperatura.
6. A leitura é validada quanto a faixa e estabilidade.
7. pH, temperatura e estado da medição são exibidos no LCD e enviados pela serial.
8. O ciclo se repete em intervalo controlado.

## 11. Testes recomendados

- conferir a tensão de 5 V com multímetro;
- executar scanner I²C e validar o endereço do LCD;
- comparar o MAX6675 com um termômetro de referência;
- registrar a saída analógica nos tampões 4,00, 7,00 e 10,00;
- verificar se a relação tensão × pH é aproximadamente linear;
- repetir cada ponto para estimar repetibilidade e tempo de estabilização;
- testar ruído com LCD/backlight ligados e desligados;
- testar desligamento e religamento para confirmar persistência da calibração;
- comparar o protótipo com um pHmetro calibrado;
- avaliar deriva durante 10 a 30 minutos em tampão estável.

## 12. Segurança, uso e manutenção

Este protótipo não deve ser tratado como instrumento clínico, laboratorial certificado, ambiental regulamentado ou de controle de processo crítico sem validação metrológica, análise de risco e adequação às normas aplicáveis.

A sonda deve permanecer hidratada em solução de armazenamento recomendada pelo fabricante. Após cada uso, deve ser enxaguada sem abrasão. Tampões contaminados não devem retornar ao frasco original. A calibração deve ser repetida periodicamente e sempre que houver troca de sonda, longa inatividade, limpeza ou suspeita de deriva.

## 13. Conclusão

O projeto reúne os elementos essenciais de um medidor didático de pH: transdução eletroquímica, condicionamento analógico, aquisição por microcontrolador, medição de temperatura, interface local e armazenamento de calibração. A montagem é apropriada para estudo e evolução de protótipo, mas a versão atual do firmware ainda requer correção matemática, integração efetiva dos parâmetros de calibração, validação da EEPROM e tratamento de falhas antes de produzir resultados confiáveis.

## 14. Referências técnicas básicas

- Repositório do projeto: <https://github.com/marcelomaurin/ph>
- Página citada pelo README original: <https://www.usinainfo.com.br/blog/sensor-de-ph-arduino-como-calibrar-e-configurar/>
- Equação de Nernst e fundamentos de eletroquímica em livros-texto de química analítica e eletroquímica.
- Folhas de dados do microcontrolador da placa Arduino, do MAX6675, do módulo LCD I²C e do condicionador de pH efetivamente utilizados na montagem.
