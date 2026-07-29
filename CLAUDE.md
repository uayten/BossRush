# BossRush

Projeto Unreal Engine 5.8. A lógica de jogo vive majoritariamente em Blueprint
(`Content/BossRush/`), com um módulo C++ em `Source/BossRush/`.

## Quem edita o quê

O dono do projeto não é programador e edita Blueprint visualmente. Isso define
onde cada coisa deve morar:

- **Blueprint** — tudo que ele quer ajustar com o olho: UI e widgets, wiring de
  habilidades (GAS), tuning, timing, feel.
- **C++** — o que ele nunca vai abrir: matemática, cálculo de atributos,
  estruturas de dados, encanamento de sistema.

Na dúvida entre os dois, pergunte se aquilo é algo que ele vai querer mexer
sozinho depois. Se sim, Blueprint.

## Ao listar nodes de Blueprint

Sempre entregue no formato do NodeScribe (`Plugins/NodeScribe/Docs/FORMATO.md`),
dentro de um bloco de código. Ele cola direto no painel do plugin, que transforma
em nodes reais. Prosa descrevendo nodes ("adicione um Get Player Controller e
ligue a saída em...") obriga ele a montar na mão — não faça isso.

Resumo do formato:

```
evento BeginPlay
pc = Get Player Controller
Branch (Condition = $bAtivo)
  verdadeiro:
    Print String (In String = "ligado")
  falso:
    Print String (In String = "desligado")
```

- uma linha = um node; `nome =` guarda a saída; `$nome` referencia
- argumentos entre parênteses, por nome de pino ou por posição
- ramos por indentação sob um rótulo (`verdadeiro:`, `falso:`, `corpo:`)

**Nunca chute um asset.** Se o node precisa de um mesh, som, material,
GameplayTag ou classe que você não tem como saber, escreva `?` no pino:

```
Spawn Sound 2D (Sound = ?)
```

O pino fica vazio, o Blueprint não compila, e a escolha aparece para ele em vez
de virar um bug silencioso de playtest. O mesmo vale para valores de
balanceamento — sugira um número, mas diga que é sugestão.
