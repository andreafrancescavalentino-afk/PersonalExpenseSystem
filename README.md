# Personal Expense System

Progetto C++ console per la gestione di spese personali e budget usando MySQL Server.

## Apertura progetto

Aprire con Visual Studio:

```text
PersonalExpenseSystem.sln
```

---

# Requisiti

Installare:

- Visual Studio Community con supporto C++
- MySQL Server 9.7
- MySQL Connector/C++ 9.7
- MySQL Workbench 8.0

---

# Configurazione Visual Studio

## Include aggiuntivi

```text
C:\Program Files\MySQL\Connector C++ 9.7\include
```

## Librerie aggiuntive

```text
C:\Program Files\MySQL\Connector C++ 9.7\lib64\vs14
```

## Dipendenze aggiuntive

```text
mysqlcppconnx.lib
```

---

# DLL richiesta

Copiare:

```text
mysqlcppconnx-2-vs14.dll
```

dentro:

```text
x64\Release
```

oppure:

```text
x64\Debug
```

La DLL si trova normalmente in:

```text
C:\Program Files\MySQL\Connector C++ 9.7\lib64\vs14
```

---

# Configurazione database

Aprire:

```text
Database.cpp
```

e modificare:

```cpp
inline const std::string PASSWORD = "password";
```

inserendo la password reale di MySQL.

---

# Struttura progetto

```text
PersonalExpenseSystem/

├── Docs/
│   └── README.md
│   
├── Source Files/
│   ├── Database.cpp
│   └── main.cpp
│
├── SQL/
│   ├── schema.sql
│   └── sample_data.sql
│
├── PersonalExpenseSystem.sln
└── PersonalExpenseSystem.vcxproj
```

---

# Avvio

1. Aprire `PersonalExpenseSystem.sln`
2. Inserire la password MySQL in `Database.cpp`
3. Verificare che MySQL Server sia avviato
4. Compilare in modalità:

```text
Release | x64
```

5. Premere:

```text
CTRL + F5
```

---

# Creazione automatica database

All'avvio il programma:

- crea automaticamente il database;
- crea automaticamente le tabelle;
- inserisce automaticamente dati di esempio.

Gli script SQL usati sono:

```text
SQL/schema.sql
SQL/sample_data.sql
```

---

# Database utilizzato

```text
gestione_spesa
```

---

# Funzionalità

- Gestione categorie
- Inserimento spese
- Definizione budget mensile
- Visualizzazione report

---

# Tabelle principali

- categorie
- spese
- budgets