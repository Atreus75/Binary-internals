# ELF (Executable and Linkable Format)

## Formato

O ELF é um formato de arquivo binário que serve como um "manual de execução" para o sistema operacional, descrevendo todos os detalhes necessários para a execução correta de um certo programa (ou biblioteca, objeto etc). Isto vai desde qual loader utilizar, arquitetura do processador, até como devem ser armazenadas as informações e dados na memória. Abaixo pode-se observar o modelo geral de um arquivo binário ELF:

![Executable and Linkable Format - Wikipedia](https://upload.wikimedia.org/wikipedia/commons/7/77/Elf-layout--en.svg)

Vamos agora destrinchar melhor estas partes.

### Headers

```c
typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf64_Half      e_type;
        Elf64_Half      e_machine;
        Elf64_Word      e_version;
        Elf64_Addr      e_entry;
        Elf64_Off       e_phoff;
        Elf64_Off       e_shoff;
        Elf64_Word      e_flags;
        Elf64_Half      e_ehsize;
        Elf64_Half      e_phentsize;
        Elf64_Half      e_phnum;
        Elf64_Half      e_shentsize;
        Elf64_Half      e_shnum;
        Elf64_Half      e_shstrndx;
} Elf64_Ehdr;
```

Os primeiros 64 bytes (em sistemas x64) de um ELF fornecem metadados sobre o mesmo. É lida e utilizada na primeira etapa da execução de um programa, pois fornece ao Kernel informações como:

* **e_ident (identificação)**: Contém informações de identificação do arquivo.

  |     **Nome**      | Offset (em bytes) |           **Valor**            |                        **Descrição**                         |
  | :---------------: | :---------------: | :----------------------------: | :----------------------------------------------------------: |
  |   Magic Number    |       0 - 3       | 7f 45 4c 46 (0x7f 'E' 'L' 'F') |               Assinatura do arquivo como ELF.                |
  | Classe do Arquivo |         4         |             1 ou 2             |                  1 para ELF32, 2 para ELF64                  |
  |    Endianness     |         5         |             1 ou 2             |              1 = little-endian, 2 = big-endian               |
  | Versão da Header  |         6         |               1                |        Versão do cabeçalho do arquivo, geralmente 1.         |
  |        ABI        |         7         |            0 a 255             | Qual ABI o binário suporta:<br />0 - System V<br />3 - Linux<br />6 - Solaris etc. |
  |   Versão da ABI   |         8         |            0 - 255             |             Versão da ABI, geralmente ignorado.              |
  |      Padding      |       9-15        |               0                |               Bits reservados para uso futuro                |
  |                   |                   |                                |                                                              |
  |                   |                   |                                |                                                              |

  A classe do arquivo e seu endianness tem importância especial nesta parte das headers, pois descrevem ao **loader** ou qualquer outro leitor a maneira correta de ler os dados ali presentes, de modo que não se tente extrair palavras de 32 bits de um ELF64 e vice-versa, ou acabe-se lendo informações de trás-pra-frente por interpretar um endianness incorreto.

* **e_type**: Define o tipo de arquivo ELF:

  * | Valor  | Constante | Descrição                                                   |
    | :----- | :-------- | :---------------------------------------------------------- |
    | `0x00` | `ET_NONE` | Tipo desconhecido                                           |
    | `0x01` | `ET_REL`  | **Arquivo objeto relocável** (`.o`, usado durante linkagem) |
    | `0x02` | `ET_EXEC` | **Executável** (endereços fixos, não-PIE)                   |
    | `0x03` | `ET_DYN`  | **Objeto compartilhado** (`.so` ou executável PIE)          |
    | `0x04` | `ET_CORE` | **Core dump** (arquivo de despejo após crash)               |

  A header **e_type**, pra além de um ELF genérico, define de qual tipo específico este arquivo se trata. Abaixo, está uma descrição do significado de cada valor possível para esta header.

  #### ET_REL (Objeto Relocável)

  Trata-se de um arquivo já compilado porém ainda não linkado à suas referências externas, como objetos e bibliotecas. Pertence à uma das últimas etapas de compilação (como do GCC) onde as referências necessárias a um arquivo são adicionadas a ele. Portanto, contém códigos e dados, mas com símbolos não resolvidos. Não é executável diretamente.

  #### ET_EXEC (Executável)

  Um arquivo diretamente executável clássico. Contém código, dados, e símbolos totalmente resolvidos e já linkados no código. Não é um Position-Independent-Executable (PIE), ou seja, teoricamente exige endereços específicos e pré-definidos na memória RAM - o que pode ser um problema se o endereço já estiver ocupado, mas estas questões terão seu próprio espaço mais adiante.

  #### ET_DYN (Objeto Compatilhado)

  Diferentemente do anterior, este tipo de ELF é **PIE**, ou seja, tem suporte a endereçamento aleatório (apenas cito aqui esta característica como motivação para leitura do tópico específico sobre o tema). Possui duas funções principais:

  * **Objeto compartilhado (.so)**: Código e dados compartilhado entre mútliplos processos.
  * **Executável**: Código diretamente executável, moderno e com endereçamento de posição independente. O tipo mais comum de executável nos dias de hoje.

  #### ET_CORE (Core Dump)

  Arquivo criado durante a falha de um processo para registrar o estado de sua memória e da CPU, guardando o estado dos registradores, conteúdo da memória entre outros. Geramente utilizado em debugging pós-mortem.

* **e_machine**: Arquitetura de destino do binário:

  * | Valor  | Constante    | Arquitetura                |
    | :----- | :----------- | :------------------------- |
    | `0x00` | `EM_NONE`    | Nenhuma (não especificada) |
    | `0x03` | `EM_386`     | Intel 80386 (x86)          |
    | `0x3E` | `EM_X86_64`  | AMD x86-64                 |
    | `0x28` | `EM_ARM`     | ARM                        |
    | `0xB7` | `EM_AARCH64` | ARM 64-bit (AArch64)       |
    | `0x08` | `EM_MIPS`    | MIPS                       |
    | `0x14` | `EM_PPC`     | PowerPC                    |
    | `0x15` | `EM_PPC64`   | PowerPC 64-bit             |
    | `0xF3` | `EM_RISCV`   | RISC-V                     |


* **e_version**: Versão do objeto:
  * `1` = `EV_CURRENT` (versão atual, quase sempre 1)
* **e_entry**: Endereço virtual de entrada da primeira instrução do programa a ser executada. Possui o offset para o primeiro endereço carregado no registrador **EIP/RIP** (ponteiro de instrução). Em executáveis, é o endereço do função ***_start*** do assembly no binário. Em objetos e bibliotecas, é sempre 0.

Estas são apenas algumas informações possíveis, mas existem muito mais delas. Para observar as headers de um binário, podemos usar o ***readelf***:

```bash
$ readelf -h programa

ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN (Position-Independent Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x1040
  Start of program headers:          64 (bytes into file)
  Start of section headers:          13976 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         14
  Size of section headers:           64 (bytes)
  Number of section headers:         30
  Section header string table index: 29
```

Os valores de offset de cada informação descrevem em quantos bytes adentro do arquivo encontram-se as informações.

### Program Headers Table

Durante a análise de um binário é muito comum que se pense em seções como *.text, .data, .rodata* etc, que são de fato unidades relevantes à execução do programa. O sistema operacional, no entanto, não entende o programa através de seções, mas sim enquanto uma porção de **segmentos**. Segmentos são grandes agrupamentos de seções, não presentes fisicamente no arquivo (como as próprias seções), mas logicamente descritas na **Program Headers Table**. 

Cada entrada no Program Headers Table do arquivo ELF descreve um segmento a partir de:

* Quais seções ele abrange
* Quais as permissões (R, W, X)
* Em qual endereço virtual ele deve ser armazenado

É a partir destas entradas na Program Headers que o sistema operacional "pega" grandes partes do arquivo (os segmentos) e os mapeia na memória rapidamente, de forma que as seções sejam agrupadas por conveniência (mesmas permissões, localidade próxima etc).

A program headers table é uma parte opcional do binário, uma vez que nem sempre o ELF representa um programa a ser executado como processo, mas também objetos e bibliotecas.

No ELF, os segmentos não existem como entidades independentes no arquivo. Eles são regiões lógicas de memória descritas pelas entradas da Program Header Table, que contêm as informações de mapeamento, permissões e endereços virtuais. A presença de Program Headers depende do tipo do ELF, sendo necessária apenas quando o arquivo é destinado a execução ou carregamento em memória. Um exemplo informal de entrada na program headers:

> ***"Pegue o segmento do arquivo começa no byte 42098 e termina no byte 81739, que possui as permissões de leitura e execução (RX), e armazene no endereço virtual 0x4befbef3."***

Note que no exemplo acima não foram citadas seções em nenhum momento, pois é assim que o SO (não estamos falando da CPU no momento) entende o programa: segmentos a serem mapeados na memória.

Podemos visualizar as **program headers** de um ELF a partir do readelf:

```bash
$ readelf -l program

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x0000000000000310 0x0000000000000310  R      0x8
  INTERP         0x00000000000003b4 0x00000000000003b4 0x00000000000003b4
                 0x000000000000001c 0x000000000000001c  R      0x1
      [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000640 0x0000000000000640  R      0x1000
  LOAD           0x0000000000001000 0x0000000000001000 0x0000000000001000
                 0x0000000000000161 0x0000000000000161  R E    0x1000
  LOAD           0x0000000000002000 0x0000000000002000 0x0000000000002000
                 0x00000000000000dc 0x00000000000000dc  R      0x1000
  LOAD           0x0000000000002dd0 0x0000000000003dd0 0x0000000000003dd0
                 0x0000000000000248 0x0000000000000250  RW     0x1000
  DYNAMIC        0x0000000000002de0 0x0000000000003de0 0x0000000000003de0
                 0x00000000000001e0 0x00000000000001e0  RW     0x8
  NOTE           0x0000000000000350 0x0000000000000350 0x0000000000000350
                 0x0000000000000040 0x0000000000000040  R      0x8
  NOTE           0x0000000000000390 0x0000000000000390 0x0000000000000390
                 0x0000000000000024 0x0000000000000024  R      0x4
  NOTE           0x00000000000020bc 0x00000000000020bc 0x00000000000020bc
                 0x0000000000000020 0x0000000000000020  R      0x4
  GNU_PROPERTY   0x0000000000000350 0x0000000000000350 0x0000000000000350
                 0x0000000000000040 0x0000000000000040  R      0x8
  GNU_EH_FRAME   0x0000000000002018 0x0000000000002018 0x0000000000002018
                 0x0000000000000024 0x0000000000000024  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000002dd0 0x0000000000003dd0 0x0000000000003dd0
                 0x0000000000000230 0x0000000000000230  R      0x1

 Section to Segment mapping:
  Segment Sections...
   00
   01     .interp
   02     .note.gnu.property .note.gnu.build-id .interp .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt
   03     .init .plt .text .fini
   04     .rodata .eh_frame_hdr .eh_frame .note.ABI-tag
   05     .init_array .fini_array .dynamic .got .got.plt .data .bss
   06     .dynamic
   07     .note.gnu.property
   08     .note.gnu.build-id
   09     .note.ABI-tag
   10     .note.gnu.property
   11     .eh_frame_hdr
   12
   13     .init_array .fini_array .dynamic .got
```



### Seções

As seções são partes de fato existentes e contínuas no arquivo ( agrupadas logicamente em segmentos pelo kernel), que organizam informações a serem utilizadas diretamente pela CPU no momento de execução do programa como:

#### .text

Nesta seção, estão armazenadas **todas** as instruções (contidas em funções) do programa a serem executadas pela CPU, incluindo a função main, headers importadas, includes, bibliotecas e funções nativas e funções do compilador. Podemos visualizar legivelmente esta seção utilizando o *objdump* da seguinte forma:

```bash
$> objdump -d [programa]
```

ou especificando a ISA desejada para visualizar:

```bash
$> objdump -dM intel64 [programa]
```

O output seria algo como:

```assembly
...
Desmontagem da secção .main:
000000000000113d <main>:
    113d:       55                      push   %rbp
    113e:       48 89 e5                mov    %rsp,%rbp
    1141:       48 83 ec 10             sub    $0x10,%rsp
    1145:       be 03 00 00 00          mov    $0x3,%esi
    114a:       bf 02 00 00 00          mov    $0x2,%edi
    114f:       e8 d5 ff ff ff          call   1129 <soma>
    1154:       89 45 fc                mov    %eax,-0x4(%rbp)
    1157:       b8 00 00 00 00          mov    $0x0,%eax
    115c:       c9                      leave
    115d:       c3                      ret

Desmontagem da secção .fini:

0000000000001160 <_fini>:
    1160:       48 83 ec 08             sub    $0x8,%rsp
    1164:       48 83 c4 08             add    $0x8,%rsp
    1168:       c3                      ret
...
```

Aqui, cada nome de função é precedido pelo endereço de memória virtual em que se encontra seu início, ou seja, sua primeira instrução. É a partir destes endereços que o processador pode chamar a execução de uma função. Logo abaixo estão presentes todas as instruções que compõem a função, listadas uma em cada linha.

#### _start

Esta seção é estritamente **read-only**, ou seja, não pode ser alterada durante a execução, permanecendo imutável na memória até o fim da execução do programa.

É importante notar que a função main só existe quando o programa é compilado de um código de linguagem de alto nível (como C) ou quando a libc é utilizada. Quando o contrário acontece, a função inicial será sempre chamada de **_start**, como no exemplo a seguir:

```assembly
section .text
	global _start

_start:
	[instruções aqui]
```

A tag `global _start` serve para avisar ao linker do código que a função _start deve ser considerada.

#### .data

#### .rodata (ELF) / .rdata (PE)

Aqui se encontram dados que devem ser somente lidos e nunca alterados em tempo de execução como: strings literais, valores estáticos de inicialização de variáveis/chamada de funções, valores de variáveis globais etc.

Podemos visualizar esta seção com o objdump:

```bash
$ ./program

You're welcome :) .

$ objdump -d -j .rodata program

Contents of section .rodata:
 2000 01000200 596f7527 72652077 656c636f  ....You're welco
 2010 6d65203a 292000                      me :) .
```

