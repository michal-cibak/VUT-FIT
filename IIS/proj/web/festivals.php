<?php
    require 'functions.php';

    print_header('Festivaly');

    // 'poradatel' and above routines
    if (has_rights_of('poradatel'))
    {
        // add festival to database
        if (array_key_exists('name', $_POST) && array_key_exists('year', $_POST) && array_key_exists('venue', $_POST) && array_key_exists('from', $_POST) && array_key_exists('to', $_POST) && array_key_exists('standard', $_POST) && array_key_exists('vip', $_POST) && array_key_exists('capacity', $_POST) && array_key_exists('description', $_POST))
            add_to_database(
                array('nazev', 'rocnik', 'misto_konani', 'datum_od', 'datum_do', 'vstupne_standard', 'vstupne_vip', 'kapacita', 'popis'),
                'Festival',
                array($_POST['name'], $_POST['year'], $_POST['venue'], $_POST['from'], $_POST['to'], $_POST['standard'], $_POST['vip'], $_POST['capacity'], $_POST['description'])
            );

        // remove/add interpret from/to festival
        if (array_key_exists('details', $_GET))
        {
            // remove
            if (array_key_exists('remove', $_GET))
            {
                try {
                    $stmt = $pdo->prepare('DELETE FROM Vystupujici WHERE festival_id = ? AND interpret_id = ?');
                    $stmt->execute(array($_GET['details'], $_GET['remove']));
                }
                catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
                }
            }

            // add
            if (array_key_exists('add', $_GET))
            {
                if (array_key_exists('stage', $_GET) && array_key_exists('from', $_GET) && array_key_exists('to', $_GET))
                    add_to_database(
                        array('festival_id', 'interpret_id', 'stage', 'od', 'do'),
                        'Vystupujici',
                        array($_GET['details'], $_GET['add'], $_GET['stage'], $_GET['from'], $_GET['to'])
                    );
                else
                {
                    echo '<form>', "\n";
                    echo '<input type="hidden" name="add" value="', $_GET['add'], '"/>', "\n";
                    echo '<input type="hidden" name="details" value="', $_GET['details'], '"/>', "\n";
                    echo '<label for="stage">Stage</label><br>', "\n";
                    echo '<input type="text" name="stage" id="stage" required><br>', "\n";
                    echo '<label for="from">Od</label><br>', "\n";
                    echo '<input type="text" name="from" id="from" required><br>', "\n";
                    echo '<label for="to">Do</label><br>', "\n";
                    echo '<input type="text" name="to" id="to" required><br>', "\n";
                    echo '<input type="submit" value="Přidat">', "\n";
                    echo '</form>', "\n";
                    print_footer();
                    die();
                }
            }
        }
    }

    // webpage for details of a festival
    if (array_key_exists('details', $_GET))
    {
        // festival details
        try {
            $stmt = $pdo->prepare('SELECT * FROM Festival WHERE id = ?');
            $stmt->execute(array($_GET['details']));
            $festival = $stmt->fetch();
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        echo "<h1>${festival['nazev']}, ročník ${festival['rocnik']}</h1>\n";
        echo "<p>\n";
        echo "<dl>\n";
        echo "<b><dt>Kde</dt></b>\n";
        echo "<dd>${festival['misto_konani']}<dd><br>\n";
        echo "<b><dt>Kdy</dt></b>\n";
        echo "<dd>${festival['datum_od']} - ${festival['datum_do']}</dd><br>\n";
        echo "<b><dt>Cena za standardní vstupenku / Cena za VIP vstupenku</dt></b>\n";
        echo "<dd>${festival['vstupne_standard']} / ${festival['vstupne_vip']}</dd><br>\n";
        echo "<b><dt>Popis</dt></b>\n";
        echo "<dd>${festival['popis']}</dd><br>\n";
        echo "</dl>\n";
        echo "</p>\n";

        if (has_rights_of('poradatel'))
        {
            echo "<h1>Upravit festival</h1>\n";
            echo "<a href=edit_festival.php?edit=${festival['id']}><button>Upravit</button></a>\n";
        }

        // interprets participating
        echo "<h1>Interpreti</h1>\n";
        echo "<p>\n";
        echo "<table>\n";
        echo "<tr>";
        foreach (array('Stage', 'Interpret', 'Od', 'Do') as $column)
            echo "<th>${column}</th>";
        echo "</tr>\n";
        try {
            $stmt = $pdo->prepare('SELECT stage, interpret_id, jmeno, od, do FROM Vystupujici JOIN Interpret ON Vystupujici.interpret_id = Interpret.id WHERE festival_id = ? ORDER BY stage, od ASC');
            $stmt->execute(array($_GET['details']));
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
        while ($row = $stmt->fetch())
        {
            echo "<tr>";
            foreach (array('stage', 'jmeno', 'od', 'do') as $column)
                if ($column == 'jmeno')
                    echo '<td><a href="bands.php?details=', $row['interpret_id'], '">', $row[$column], '</a></td>';
                else
                    echo '<td>', $row[$column], '</td>';
            if (has_rights_of('poradatel'))
                echo '<td><a href="festivals.php?details=', $_GET['details'], '&amp;remove=', $row['interpret_id'], '"><button class="button button3">Odebrat</button></a></td>';
            echo "</tr>\n";
        }
        echo "</table>\n";
        echo "</p>\n";

        // buy tickets link
        echo "<h1>Koupit lístek na festival</h1>\n";
        echo "<p>\n";
        $datum = date('Y-m-d');
        $datum_do = $festival['datum_do'];
        if ($datum_do < $datum)
            echo "Festival již proběhl. Rezervovat vstupenky není možné.";
        else
            echo "<a href=tickets.php?buy=${festival['id']}><button>Nakupovat</button></a>\n";
        echo "</p>\n";

        // add interprets table
        if (has_rights_of('poradatel'))
        {
            echo "<h1>Přidat interpreta na festival</h1>\n";
            echo "<table>\n";
            echo "<tr>";
            foreach (array('Name', 'Since', 'Genre') as $column)
                echo "<th>${column}</th>";
            echo "</tr>\n";
            try {
                $rows = $pdo->query('SELECT * FROM Interpret');
            }
            catch (PDOException $e) {
                echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
            }
            while ($row = $rows->fetch())
            {
                echo "<tr>";
                foreach (array('jmeno', 'datum_vzniku', 'zanr') as $column)
                    echo '<td>', $row[$column], '</td>';
                echo '<td><a href="festivals.php?details=', $_GET['details'], '&amp;add=', $row['id'], '"><button class="button button1">Přidat</button></a></td>';
                echo "</tr>\n";
            }
            echo "</table>\n";
        }

        if (has_rights_of('pokladni'))
        {
            echo "<h1>Rezervace uživatelů</h1>";
            echo '<td><a href="stat_reserv.php?reserv=', $_GET['details'], '"><button>Spravovat rezervace</button></a></td>';
        }
        print_footer();
        die();
    }

    // webpage for all festivals - normal page

    echo '<h1>Všechny festivaly</h1>';
    try {
        $rows = $pdo->query('SELECT id, nazev, rocnik, misto_konani, datum_od, datum_do FROM Festival ORDER BY datum_od DESC');
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
    print_table(
        array('Jméno', 'Ročník', 'Místo', 'Začátek', 'Konec'),
        array('nazev', 'rocnik', 'misto_konani', 'datum_od', 'datum_do'),
        '',
        $rows,
        'festivals.php'
    );

    if (has_rights_of('poradatel'))
    {
        echo '<h1>Přidat festival</h1>';
        print_form(
            array('Název', 'Ročník', 'Místo', 'Začátek', 'Konec', 'Cena za standard', 'Cena za VIP', 'Kapacita koncertu', 'Popis'),
            array('name', 'year', 'venue', 'from', 'to', 'standard', 'vip', 'capacity', 'description'),
            'post'
        );
        echo "<p>* všechna pole jsou povinná</p>\n";
    }

    print_footer();
?>
