<?php
require 'functions.php';

print_header('Editace festivalu');

if (!has_rights_of('poradatel'))
{
    print_footer();
    die();
}


if (array_key_exists('edit', $_GET)) {
    {
        if ($_POST) {
            try {
                $festival = $pdo->prepare("SELECT * FROM Festival WHERE id = ?");
                $festival->execute(array($_GET['edit']));
                $festival = $festival->fetch();
            } catch (PDOException $e) {
                echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
            }

            if (isset($_POST['venue']) && $_POST['venue'] !== '') {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Festival SET misto_konani = ? WHERE id = ?');
                    $stmt->execute(array($_POST['venue'], $festival['id']));
                } catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
                }
            }
            if (isset($_POST['from']) && $_POST['from'] !== '') {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Festival SET datum_od = ? WHERE id = ?');
                    $stmt->execute(array($_POST['from'], $festival['id']));
                } catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
                }
            }
            if (isset($_POST['to']) && $_POST['to'] !== '') {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Festival SET datum_do = ? WHERE id = ?');
                    $stmt->execute(array($_POST['to'], $festival['id']));
                } catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
                }
            }
            if (isset($_POST['description']) && $_POST['description'] !== '') {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Festival SET popis = ? WHERE id = ?');
                    $stmt->execute(array($_POST['description'], $festival['id']));
                } catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
                }
            }


            alert('Festival byl upraven');
        }

        echo "<h1>Správa Festivalu</h1>\n";
        echo "<form method=post>
        <label for=venue>Místo<label><br>
        <input type=text name=venue id=venue><br>
        <label for=from>Začátek</label><br>
        <input type=text name=from id=from><br>
        <label for=to>Konec</label><br>
        <input type=text name=to id=to><br>
        <label for=description>Popis</label><br>
        <input type=text name=description id=description><br>
        <input type=submit value=Upravit>
        </form>\n";
        echo "<p>* prázdné pole = beze změny</p>\n";
    }
}
print_footer();
?>