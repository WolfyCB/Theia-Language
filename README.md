# Linguagem Sphen

> Uma linguagem moderna, fortemente tipada, com orientação a objetos explícita e suporte a módulos – evolução do Lua mantendo a simplicidade.

## Estado atual

**análise léxica e sintática.**  
O compilador já implementa:
- Analisador léxico completo (tokens, palavras-chave, operadores, números, strings, caracteres, comentários)
- Analisador sintático recursivo descendente
- Geração de Árvore Sintática Abstrata (AST)
- Sistema de logs e reporte de erros com cores

---

## Visão geral

Sphen combina a simplicidade sintática do Lua com tipagem estática, tuplas e um sistema de módulos. A sintaxe é limpa e usa palavras-chave em inglês (`let`, `fn`, `if`, `while`, `switch`).

### Características principais

- **Tipagem estática forte** com inferência de tipos
- **Variáveis imutáveis por padrão** (const) e mutáveis (`let`)
- **Referências** (`ref`) para aliasing
- **Funções de primeira classe** (planejado)
- **Tuplas** como tipos compostos nativos
- **Controle de fluxo** moderno: `if`/`elif`/`else`, `while`/`do-while`, `switch`/`case`/`default`
- **Módulos** com `import` e `export` (planejado)
- **Comentários de linha (`#`) e bloco (`#[ ... ]#`)**
- **Operadores de intervalo**: `..` (exclusivo) e `..=` (inclusivo)

---

## Sintaxe

### Variáveis

```sphen
let x = 10            # tipo inferido como int
let y: float = 3.14   # tipo explícito
const MAX = 100       # constante
ref r = x             # referência a x
Funções
sphen
fn soma(a: int, b: int): int
    return a + b
end

fn saudacao(nome: string)
    print("Olá, " + nome)   # sem retorno explícito
end
Estruturas condicionais
sphen
if x > 0:
    print("positivo")
elif x == 0:
    print("zero")
else:
    print("negativo")
end
Laços
sphen
# while
while i < 10:
    print(i)
    i = i + 1
end

# do-while
do:
    print(i)
    i = i - 1
while i > 0:
Switch
sphen
switch valor:
    case 1:
        print("um")
    case 2:
        print("dois")
    default:
        print("outro")
end
Tuplas
sphen
let ponto = (10, 20)               # criação
fn divide_resto(a: int, b: int): (int, int)
    return (a / b, a % b)
end

let (q, r) = divide_resto(10, 3)   # desestruturação
Comentários
sphen
# comentário de linha
#[ comentário
   de múltiplas
   linhas ]#
Operadores
Categoria	Operadores
Aritméticos	+ - * / %
Atribuição	= += -= *= /= %=
Comparação	== != < > <= >=
Lógicos	and or not
Intervalo	.. (exclusivo), ..= (inclusivo)
Referência	ref
Tipos primitivos
Tipo	Descrição
int	Inteiro com sinal (64 bits)
uint	Inteiro sem sinal (64 bits)
float	Ponto flutuante (64 bits)
double	Sinônimo de float
char	Caractere (8 bits)
uchar	Caractere sem sinal
bool	Booleano (true/false)
string	Sequência de caracteres (imutável)
short / ushort	Inteiros de 16 bits
long / ulong	Inteiros de 64 bits (sinônimos)
size / usize	Tamanhos (mesmo que uint/int)
Módulos (planejado)
sphen
# math.sp
export fn sqrt(x: float): float ...

# main.sp
import "math.sp" as math
let raiz = math.sqrt(2.0)
Compilação
Requisitos
CMake 3.25+

Compilador C com suporte a C17 (GCC, Clang, MSVC)

Passos
git clone https://github.com/Punker-Corporation/Sphen-Language.git
cd Sphen-Language
mkdir build && cd build
cmake ..
make
O executável Sphen será criado em build/.

Uso
./Sphen caminho/para/arquivo.sp
Atualmente, o compilador apenas exibe erros léxicos/sintáticos ou, em modo de depuração, a lista de tokens e a AST gerada.

Estrutura do projeto
text
.
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.c                # Ponto de entrada
    ├── lexer.c / lexer.h     # Analisador léxico
    ├── parser.c / parser.h   # Analisador sintático
    ├── ast.c / ast.h         # Árvore Sintática Abstrata
    ├── tokens.c / tokens.h   # Tabelas de tokens e hash
    ├── str.c / str.h         # Strings dinâmicas
    ├── debug.c / debug.h     # Log colorido e reporte de erros
