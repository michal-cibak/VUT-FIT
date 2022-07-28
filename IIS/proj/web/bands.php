<?php
    require 'functions.php';

    print_header('Interpreti');

    // 'poradatel' and above routines
    if (has_rights_of('poradatel'))
    {
        // add interpret to database
        if (array_key_exists('name', $_POST) && array_key_exists('date', $_POST) && array_key_exists('genre', $_POST))
            add_to_database(
                array('jmeno', 'datum_vzniku', 'zanr'),
                'Interpret',
                array($_POST['name'], $_POST['date'], $_POST['genre'])
            );

        // remove/add member from/to band
        if (array_key_exists('details', $_GET))
        {
            // add
            if (array_key_exists('firstname', $_POST) && array_key_exists('lastname', $_POST) && array_key_exists('position', $_POST))
            {
                add_to_database(
                    array('jmeno', 'prijmeni', 'pozice'),
                    'Clen',
                    array($_POST['firstname'], $_POST['lastname'], $_POST['position'])
                );
                try {
                    $last_id = $pdo->lastInsertId();
                }
                catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
                }
                add_to_database(
                    array('interpret_id', 'clen_id'),
                    'Clenove',
                    array($_GET['details'], $last_id)
                );
            }

            // remove
            if (array_key_exists('remove', $_GET))
            {
                try {
                    $stmt = $pdo->prepare('DELETE FROM Clenove WHERE interpret_id = ? AND clen_id = ?');
                    $stmt->execute(array($_GET['details'], $_GET['remove']));
                    $stmt = $pdo->prepare('DELETE FROM Clen WHERE id = ?');
                    $stmt->execute(array($_GET['remove']));
                }
                catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
                }
            }
        }
    }

    // webpage for details of an interpret
    if (array_key_exists('details', $_GET))
    {
        // interpret details
        try {
            $stmt = $pdo->prepare('SELECT * FROM Interpret WHERE id = ?');
            $stmt->execute(array($_GET['details']));
            $interpret = $stmt->fetch();
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        echo "<h1>${interpret['jmeno']}</h1>\n";
        echo "<p>\n";
        echo "<dl>\n";
        echo "<b><dt>Logo</dt></b>\n";
        if (isset($interpret['logo_img']))
            echo "<dd><img src=\"images/${interpret['logo_img']}\" alt=\"Logo interpreta\" width=\"320\" /></dd><br>\n";
        else
            echo "<dd><img src=\"images/unavailable.png\" alt=\"Logo neexistuje\" width=\"320\" /></dd><br>\n";
        echo "<b><dt>Datum založení</dt></b>\n";
        echo "<dd>${interpret['datum_vzniku']}</dd><br>\n";
        echo "<b><dt>Žánr</dt></b>\n";
        echo "<dd>${interpret['zanr']}</dd><br>\n";
        echo "</dl>\n";
        echo "</p>\n";

        if (has_rights_of('poradatel'))
        {
            echo "<h1>Upravit interpreta</h1>\n";
            echo "<a href=edit_interpret.php?edit=${interpret['id']}><button>Upravit</button></a>\n";
            echo "<a href=upload.php?interpret=${interpret['id']}><button>Nahrát logo</button></a>\n";
        }

        // members
        echo "<h1>Členové</h1>\n";
        echo "<p>\n";
        echo "<table>\n";
        echo "<tr>";
        foreach (array('Jméno', 'Příjmení', 'Pozice') as $column)
            echo "<th>${column}</th>";
        echo "</tr>\n";
        try {
            $stmt = $pdo->prepare('SELECT * FROM Clenove JOIN Clen ON Clenove.clen_id = Clen.id WHERE interpret_id = ?');
            $stmt->execute(array($_GET['details']));
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        while ($row = $stmt->fetch())
        {
            echo "<tr>";
            foreach (array('jmeno', 'prijmeni', 'pozice') as $column)
                echo '<td>', $row[$column], '</td>';
            if (has_rights_of('poradatel'))
                echo '<td><a href="bands.php?details=', $_GET['details'], '&amp;remove=', $row['clen_id'], '"><button class="button button3">Odebrat</button></a></td>';
            echo "</tr>\n";
        }
        echo "</table>\n";
        echo "</p>\n";

        // add members table
        if (has_rights_of('poradatel'))
        {
            echo "<h1>Přidat člena</h1>\n";
            print_form(
                array('Jméno', 'Příjmení', 'Pozice'),
                array('firstname', 'lastname', 'position'),
                'post'
            );
            echo "<p>* všechna pole jsou povinná</p>\n";
        }

        print_footer();
        die();
    }

    // webpage for all intrprets - normal page

    // table with interprets
    echo "<h1>Seznam interpretů</h1>\n";
    try {
        $rows = $pdo->query('SELECT id, jmeno, datum_vzniku, zanr FROM Interpret ORDER BY jmeno ASC');
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
    print_table(
        array('Jméno', 'Založeno', 'Žánr'),
        array('jmeno', 'datum_vzniku', 'zanr'),
        '',
        $rows,
        'bands.php'
    );

    // form for adding a new interpret
    if (has_rights_of('poradatel'))
    {
        echo "<h1>Přidat interpreta</h1>\n";
        print_form(
            array('Jméno', 'Založeno', 'Žánr'),
            array('name', 'date', 'genre'),
            'post'
        );
        echo "<p>* všechna pole jsou povinná</p>\n";
    }

    print_footer();
?>
