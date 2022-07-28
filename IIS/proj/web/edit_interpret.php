<?php
require 'functions.php';

print_header('Editace interpreta');

if (!has_rights_of('poradatel'))
{
    print_footer();
    die();
}

if (array_key_exists('edit', $_GET)) {
    {
        if ($_POST) {
            try {
                $bands = $pdo->prepare("SELECT * FROM Interpret WHERE id = ?");
                $bands->execute(array($_GET['edit']));
                $bands = $bands->fetch();
            } catch (PDOException $e) {
                echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
            }

            if (isset($_POST['genre']) && $_POST['genre'] !== '') {
                try {
                    $stmt = $GLOBALS['pdo']->prepare('UPDATE Interpret SET zanr = ? WHERE id = ?');
                    $stmt->execute(array($_POST['genre'], $bands['id']));
                } catch (PDOException $e) {
                    echo '<h2>Spojení s databází selhalo: ' . $e->getMessage() . '</h2>';
                }
            }
            alert('Interpret byl upraven');
        }

        echo "<h1>Správa Interpreta</h1>\n";
        echo "<form method=post>
        <label for=genre>Žánr<label><br>
        <input type=text name=genre id=genre><br>
        <input type=submit value=Upravit>
        </form>\n";
        echo "<p>* prázdné pole = beze změny</p>\n";
    }
}
print_footer();
?>
