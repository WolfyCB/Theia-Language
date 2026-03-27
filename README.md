# Linguagem Sphen

> Uma linguagem moderna, fortemente tipada, com orientação a objetos explícita e suporte a módulos – evolução do Lua mantendo a simplicidade.

## Estado atual

**Análise léxica e sintática.**  
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
- **Variáveis imutáveis** (`const`) e **mutáveis** (`let`)
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
let x = 10                     # tipo inferido como int
let y: float = 3.14            # tipo explícito
const MAX = 100                # constante
ref r = x                      # referência a x
let x, y, z = 10, 'b', 3.14    # multiplas declarrações
let (a, b) = (15, -5)          # desestruturação de uma tuple
let x:char, (a, b:int) = 'x', ("abc", 42) #misturado
```


### Funções
```sphen
fn soma(a: int, b: int): int
    a + b    #irá retornar a expressão automaticamente
end

fn saudacao(nome: str)
    print("Olá, " + nome)   # sem retorno explícito
end

fn preset(): int, str, (char, float)
    return 10, "ola", ('a', 3.14) #retorno multiplo
end
```

### Estruturas condicionais:
```sphen
if x > 0:
    print("positivo")
elif x == 0:
    print("zero")
else:
    print("negativo")
end
```

### Loops
```sphen
    #while
while i < 10:
    print(i)
    i = i + 1
end
    #do while
do:
    print(i)
    i = i - 1
while i > 0:
    #for
for i in 0..1=0 step 2:
    print(i)     #0, 2, 4, 6, 8, 10
end
```

### Switch
```sphen
switch valor:
    case 1:
        print("um")
        break      # sem break, cairia no próximo caso (fallthrough)
    case 2:
        print("dois")
        break
    default:
        print("outro")
end
```
#### Nota: Sphen possui fallthrough em switch. Use break para sair do caso; omissão causa execução sequencial.

### Tuplas
```sphen
let ponto = (10, 20)               # variavel tem seu tipo inferido como uma tuple
let ponto2:(int, int) = (10, 20)   # varavel tem seu tipo explicitamente como uma tuple

fn divide_resto(a: int, b: int): (int, int)
    (a / b, a % b)
end

let (q, r) = divide_resto(10, 3)   # desestruturação
```

### Comentários:
```sphen
# comentário de linha
#[ comentário
   de múltiplas
   linhas ]#
```

---

## Operadores:
- **Aritméticos:**	       `+ - * / %`
- **Atribuição:**	       `= += -= *= /= %=`
- **Comparação:**          `== != < > <= >=`
- **Lógicos:**	           `and or not`
- **Intervalo:**	       `..`(exclusivo), `..=` (inclusivo)

---

## Tipos primitivos
- **int:**	             Inteiro com sinal (32 bits)
- **uint:**	             Inteiro sem sinal (32 bits)
- **float:**	         Ponto flutuante (32 bits)
- **double:**	         Sinônimo de float (64 bits)
- **char:**	             Caractere (8 bits)
- **uchar:**	         Caractere sem sinal
- **bool:**	             Booleano (true/false)
- **str:**	             Sequência de caracteres (imutável)
- **short / ushort:**	 Inteiros de 16 bits
- **long / ulong:**	     Inteiros de 64 bits
- **size / usize:**	     Tamanhos (4 bytes em sistmeas 32 bits e 8 bytes em sistemas 64 bits)

---

## Compilação
### Requisitos:
CMake 3.25+<br>
Compilador C com suporte a C17 (GCC, Clang, MSVC)<br>

### Passos:
```
git clone https://github.com/Punker-Corporation/Sphen-Language.git
cd Sphen-Language
mkdir build && cd build
cmake ..
make
```


Uso: `./Sphen caminho/para/arquivo.sp`<br>

Nota:Atualmente, o compilador apenas exibe erros léxicos/sintáticos ou, em modo de depuração, a lista de tokens e a AST gerada.



