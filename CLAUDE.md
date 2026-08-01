# BossRush

Projeto Unreal 5.8.1. O usuário é não-programador: ao pedir ação no editor,
diga o que clicar, não o que digitar em terminal.

## NodeScribe

`Plugins/NodeScribe` (submódulo de github.com/uayten/NodeScribe) é como você lê
e escreve neste projeto. Tudo abaixo passa por ele.

**A especificação do formato de grafo é
[`Plugins/NodeScribe/Docs/FORMATO.md`](Plugins/NodeScribe/Docs/FORMATO.md).
Leia esse arquivo antes de escrever ou interpretar um grafo** — as partes que
não dá para adivinhar (`$nome.Pino`, `Classe.Funcao`, `evento X de Y`, rótulos
indentados, `?` como buraco declarado) estão lá.

Esse é o **único** formato para entregar grafo neste projeto. Nada de lista
numerada de passos nem de descrever node de dado em prosa — o parser não lê
isso, e o objetivo é a resposta ser colável.

Na barra do editor de Blueprint há **Colar**, **Copiar selecionado** e **Copiar
grafo inteiro**. Quando o usuário colar texto nesse formato, é saída de
**Copiar grafo inteiro**. Diagnósticos vão para o Message Log, canal
*NodeScribe*.

## As ferramentas, e para que serve cada uma

Toolset MCP `nodescribe_toolset.toolsets.graph.NodeScribeTools`:

| ferramenta | para quê |
|---|---|
| `read_object(target, filter)` | propriedades de qualquer objeto, classe, CDO, ator ou asset. Filtro vazio = só o que difere do padrão |
| `write_object(target, text)` | aplica uma ficha. Lista de mudanças, não estado final |
| `read_graph(graph)` | um grafo de Blueprint inteiro como texto |
| `write_graph(graph, text)` | cria nodes a partir do texto |
| `read_tags(filter)` | as Gameplay Tags declaradas, uma por linha |
| `write_tags(text, source)` | cria tags. `source` é o ini (`BossRush.ini`) |
| `create_asset(path, parent)` | asset vazio. Só dentro de `/Game/` |
| `get_format_docs()` | a especificação do formato |
| `save_all_and_quit()` | salva e fecha o editor |

`read_object` e `write_object` mudam de forma sozinhos conforme o alvo:
**Blackboard** vira lista de `chave Nome : Tipo`, **Behavior Tree** vira árvore
indentada. Não há ferramenta separada para esses.

Procurando uma propriedade específica, passe o filtro na **mesma** chamada —
`read_object(alvo, "walk")` devolve nome, tipo e valor. Nunca leia tudo para
depois procurar.

## Como referenciar um asset

`refPath` é caminho de objeto: `/Game/Pasta/Asset.Asset`.

Para grafo, some `:NomeDoGrafo` — e **o nome do grafo não é adivinhável.**
`EventGraph` funciona quase sempre, mas o `GA_GolemSalto` usa
`Gameplay Ability Graph`, com espaços, enquanto o `GA_ChuvaDePedras` usa
`EventGraph`. Foram criados por caminhos diferentes.

**Não fique tentando nomes.** `read_object` num Blueprint lista os grafos:

```
ficha GA_GolemSalto (GameplayAbility)
# grafos: Gameplay Ability Graph
```

## Regras de trabalho

**Leia antes de propor.** É para isso que essas ferramentas existem. Propor
mecânica sem ler o asset produz solução que ignora metade do que já está feito
— e neste projeto quase sempre já existe um protótipo pela metade.

**Não escreva em asset de produção sem perguntar.** Rascunho vai para
`Content/BossRush/Testes/`, que já tem `BP_TesteGolem` e `BB_Teste`. Escrita
que o usuário pediu explicitamente pode seguir; escrita que você achou boa
ideia, pergunte.

**`create_asset` existe no NodeScribe**, e só lá — o toolset nativo tem
`duplicate`, `move` e `delete`, e não tem criação. Nunca sobrescreve, e só cria
dentro de `/Game/`. O asset fica sujo, sem salvar, como qualquer recém-criado.

**Medido, e não vale trazer para o plugin:** criar, mover ou duplicar asset
pelo toolset nativo custa ~4.000 tokens de descoberta uma vez por sessão e ~50
por chamada, e não tem payload gordo para comprimir. Ver *Criar asset* no README
do plugin.

## O ciclo com o editor

Você abre e fecha a Unreal sozinho.

```powershell
Start-Process "E:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Unreal Projects\BossRush\BossRush.uproject"
```

Fechar: `save_all_and_quit()` pelo MCP, ou — se o MCP estiver fora do ar —
escrevendo `quit` em `Saved/NodeScribe/comando.txt`. O plugin vigia esse
arquivo. A resposta sai em `resposta.txt` ao lado.

**Compilar o plugin exige o editor fechado**, e o Live Coding bloqueia a build
inteira enquanto ele estiver aberto — nem mostra erro de compilação. O ciclo
completo está em *Desenvolvimento*, no README do plugin.

## Idiomas deste projeto

Siga estes ao entregar grafo novo — consistência com o que existe vale mais
que pureza.

**Habilidade GAS com repetição** (`GA_ChuvaDePedras`, `GA_GolemSalto`):
`ActivateAbility` → `Commit Ability` → Branch → evento customizado que se
rechama por `Set Timer by Event`. O contador é variável do próprio GA. Antes do
`End Ability`, sempre `Clear and Invalidate Timer by Handle` — se a habilidade
for cancelada no meio, o timer não pode ficar solto.

**Cooldown é Gameplay Effect, não timer no grafo.** `Duration Policy = Has
Duration`, a duração em `Scalable Float`, e um componente **Grant Tags to
Target Actor** concedendo a tag de cooldown. No GA, `Cooldown Gameplay Effect
Class` aponta para ele. O `Commit Ability` aplica.

**BTTask de Blueprint** (`BTTask_Patrulhar`): `evento ExecuteAI` →
`$executeAI.ControlledPawn` → ação → `Finish Execute (bSuccess = true)`. Para
task que espera algo terminar, `evento TickAI` decide quando chamar
`Finish Execute`.

**AIController** (`AIC_Golem`): no `Possess`, `Delay` → `Run Behavior Tree` →
preenche as chaves iniciais do blackboard com `Set Value as Object`.

## Estado da IA do Golem

Assets em `Content/BossRush/IA/Golem/Behavior/` e
`Content/BossRush/GAS/Habilidades/Golem/`.

Funciona: `AIC_Golem` roda a `BT_Golem` e preenche `Jogador`. A árvore é um
Selector com dois Sequences — `Move To` e `Patrulhar`/`Wait`. `GA_GolemSalto`
está completo, com cooldown de 10s ligado.

**Nenhuma habilidade tem tag.** Todos os containers do `GA_Laser` estão vazios,
então ativar por tag não funciona — tem que ser por classe.

**A árvore não tem decorator nem service**, e nada nela ativa habilidade. As
chaves `Pode usar Laser?` e `Pode usar Projétil?` não são lidas por ninguém.

**`BTService_LocalizarJogador` está pela metade** — faz um trace de raio 200 e
abre um ForEachLoop vazio. Não reaproveite.

**Não confirmado:** se o `Default Starting Data` do componente `AbilitySystem`
do `BP_Golem` concede as habilidades. A ficha não consegue ler esse valor, e o
`BeginPlay` não tem `Give Ability`. Sem isso, nenhuma habilidade do Golem roda.

## Limites conhecidos das ferramentas

- **Escrever Behavior Tree ainda não existe.** Ler funciona. Mudança na árvore
  é o usuário quem faz, e você diz o que clicar.
- **`write_object` cria variável de Blueprint** (`variavel X : Tipo editavel`),
  e só apaga com a palavra `apagar`. **Componente ele não cria.**
- **`write_tags` não apaga nem renomeia tag**, e com `source` vazio grava em
  `DefaultGameplayTags.ini` — as tags deste projeto moram em
  `Config/Tags/BossRush.ini`, então passe `source` para não espalhar.
- **Struct grande** abre e mostra só o membro que mudou; o que nem assim cabe
  sai nomeado num `[nota]`, e você pede pelo nome para ver inteiro.
- **Cast com continuação não volta igual.** No `Gameplay Ability Graph` do
  `GA_GolemSalto`, a cadeia depois do `Cast to Character` sai sem rótulo — o
  texto lido não recria aquela ligação se colado de volta. Confira antes de
  reescrever um grafo que tenha Cast no meio da execução.
