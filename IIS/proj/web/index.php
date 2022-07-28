<?php
    require 'functions.php';

    print_header('Index');

    // show current and upcoming festivals
    $date = date('Y-m-d');
    try {
        $rows = $pdo->query("SELECT id, nazev, datum_do FROM Festival WHERE datum_od <= '$date' AND datum_do >= '$date' ORDER BY datum_do ASC"); // safe to use - date is retuned by a function in proper fromat, so it can't cause issues
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
    if ($rows->rowCount())
    {
        echo '<h1>Probíhající festivaly</h1>', "\n";
        print_table(
            array('Název', 'Konec festivalu'),
            array('nazev', 'datum_do'),
            '',
            $rows,
            'festivals.php'
        );
    }
    try {
        $rows = $pdo->query("SELECT id, nazev, rocnik, datum_od, datum_do FROM Festival WHERE datum_od > '$date' ORDER BY datum_od ASC"); // safe to use - date is retuned by a function in proper fromat, so it can't cause issues
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
    if ($rows->rowCount())
    {
        echo '<h1>Nádcházející festivaly</h1>', "\n";
        print_table(
            array('Název', 'Ročník', 'Začátek festivalu', 'Konec festivalu'),
            array('nazev', 'rocnik', 'datum_od', 'datum_do'),
            '',
            $rows,
            'festivals.php'
        );
    }

    print_footer();
?>
