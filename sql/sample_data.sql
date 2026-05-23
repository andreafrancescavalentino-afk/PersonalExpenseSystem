USE gestione_spesa;

INSERT IGNORE INTO categorie(name) VALUES
('Alimentari'), ('Trasporti'), ('Casa'), ('Svago'), ('Salute');

INSERT INTO spese(data_spesa, importo, id_categoria, descrizione) VALUES
('2025-01-02', 45.70, (SELECT id FROM categorie WHERE name = 'Alimentari'), 'Spesa supermercato'),
('2025-01-04', 25.00, (SELECT id FROM categorie WHERE name = 'Trasporti'), 'Abbonamento bus'),
('2025-01-15', 25.00, (SELECT id FROM categorie WHERE name = 'Alimentari'), 'Pranzo');

INSERT INTO budgets(mese, id_categoria, importo) VALUES
('2025-01', (SELECT id FROM categorie WHERE name = 'Alimentari'), 60.00),
('2025-01', (SELECT id FROM categorie WHERE name = 'Trasporti'), 40.00)
ON DUPLICATE KEY UPDATE importo = VALUES(importo);