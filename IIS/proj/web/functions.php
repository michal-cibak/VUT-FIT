<?php
    session_save_path('sessions');
    session_start(); // MUST be at the start!
    header('Content-Type: text/html; charset=utf-8');
    try {
        $pdo = new PDO("mysql:host=localhost;dbname=xcibak00;port=/var/run/mysql/mysql.sock", 'xlogin00', 'p4ssw0rd', array(PDO::MYSQL_ATTR_INIT_COMMAND => 'SET NAMES utf8')); // change login and password
    }
    catch (PDOException $e) {
       echo '$e->getMessage()';
    }

// ----------
function print_header($title)
{
?>
<!DOCTYPE html>
<html lang="cs">

<head>
    <meta content="text/html" charset="utf-8">
    <title><?php echo $title; ?></title>

<style>
body {
  margin: 0;
  font-family: Arial, Helvetica, sans-serif;
}

.topnav {
  overflow: hidden;
  background-color: #333;
}

.main_body {
  padding: 0px 5px;
}

.topnav a, .user {
  float: left; /* for whatever reason, this makes the navigation bar taller */
  color: #f2f2f2;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
  font-size: 17px;
}

.topnav a:hover {
  background-color: #bfe7dc;
  color: black;
}

.topnav a:active {
  background-color: #4CAF50;
  color: white;
}

table {
  font-family: arial, sans-serif;
  border-collapse: collapse;
  width: 100%;
}

td, th {
  border: 1px solid #dddddd;
  text-align: left;
  padding: 8px;
}

tr:nth-child(even) {
  background-color: #dddddd;
}

.button {
    border: none;
    color: white;
    text-align: center;
    display: inline-block;
}

.button1 {background-color: #4CAF50;}
.button2 {background-color: #008CBA;}
.button3 {background-color: #FF4F4F;}
</style>

</head>

<body>
    <nav class="topnav">
        <span style="float:left">
            <a href="index.php">Domů</a>
            <a href="festivals.php">Festivaly</a>
            <a href="bands.php">Kapely</a>
            <?php if (has_rights_of('administrator')) echo '<a href="users.php">Uživatelé</a>'; ?>
        </span>
        <span style="float:right">
            <?php
            if (isset($_SESSION['user']))
                echo '<span class="user">Uživatel: <b>', $_SESSION['user'], '</b></span>
                <a href="edit.php">Upravit profil</a>
                <a href="tickets.php">Moje rezervace</a>
                <a href="logout.php">Odhlásit se</a>';
            else
                echo '<a href="login.php">Přihlásit se</a>
                <a href="register.php">Registrovat se</a>';
            ?>
        </span>
        <br clear: both>
    </nav>

    <div class="main_body">
<?php
}
// ----------

// ----------
function print_footer()
{
?>
    </div>
<!--
    <hr>
    <footer style="text-align:center;"><a href="index.php"><button>Zpět na hlavní stránku</button></a></footer>
-->
</body>

</html>
<?php
}
// ----------

// ----------
function print_table($header, $columns, $table, $rows=null, $details=null, $edit=null, $delete=null)
/*  Prints a table with specified columns of data.
    header - displayed names of columns
    columns - names of columns in DB table
    table - name of DB table - rows will be acquired from specified DB table unless they are provided
    rows - already acquired rows from DB table - used instead of table when provided
    details - whether column with links to a details page should be added - id column needed
    edit - whether column with edit links should be added - id column needed
    delete - whether column with delete links should be added - id column needed
*/
{
    echo "<table>\n";
    echo "<tr>";
    foreach ($header as $column)
        echo "<th>${column}</th>";
    echo "</tr>\n";

    if ($rows === null)
        try {
            $rows = $GLOBALS['pdo']->query('SELECT ' . implode(', ', $columns) . " FROM $table"); // safe to use - columns are manually passed from other files, so they can't cause issues
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }

    while ($row = $rows->fetch())
    {
        echo "<tr>";
        foreach ($columns as $column)
            echo '<td>', $row[$column], '</td>';
        if ($details !== null)
            echo '<td><a href="', $details, '?details=', $row['id'], '"><button class="button button1">Detaily</button></a></td>';
        if ($edit !== null)
            echo '<td><a href="', $edit, '?edit=', $row['id'], '"><button class="button button2">Upravit</button></a></td>';
        if ($delete !== null)
            echo '<td><a href="', $delete, '?delete=', $row['id'], '"><button class="button button3">Vymazat</button></a></td>';
        echo "</tr>\n";
    }
    echo "</table>\n";
}
// ----------

// ----------
function print_form($labels, $fields, $method=null, $action=null, $sendbtn='Přidat')
{
    echo '<form';
    if ($action !== null) echo ' action="', $action, '"';
    if ($method !== null) echo ' method="', $method, '"';
    echo ">\n";
    foreach (array_combine($labels, $fields) as $label => $field)
    {
        echo "<label for=\"${field}\">${label}</label><br>\n";
        echo "<input type=\"text\" name=\"${field}\" id=\"${field}\" required><br>\n";
    }
    echo "<input type=\"submit\" value=\"${sendbtn}\">\n";
    echo "</form>\n";
}
// ----------

// ----------
function add_to_database($columns, $table, $values)
/*  Adds a row of values into a DB table.
    columns - names of columns in DB table
    table - name of DB table
    values - row to be added into DB
*/
{
    if (count($columns) == count($values))
    {
        $qmarks = array();
        for ($i = 0; $i < count($columns); $i++)
            $qmarks[] = '?';

        try {
            $stmt = $GLOBALS['pdo']->prepare("INSERT INTO $table (" . implode(', ', $columns) . ') VALUES (' . implode(', ', $qmarks) . ')');
            $stmt->execute($values);
        }
        catch (PDOException $e) {
            echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
        }
    }
}
// ----------

// ----------
function register_user()
{
    add_to_database(
        array('prezdivka', 'heslo', 'email'),
        'Uzivatel',
        array($_POST['username'], crypt($_POST['password']), $_POST['email'])
    );
}
// ----------

// ----------
function login_user()
{
    try {
        $stmt = $GLOBALS['pdo']->prepare('SELECT heslo, opravneni FROM Uzivatel WHERE prezdivka = ?');
        $stmt->execute(array($_POST['username']));
        $user = $stmt->fetch();
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
    if (crypt($_POST['password'], $user['heslo']) == $user['heslo'])
    {
        $_SESSION['user'] = $_POST['username'];
        $_SESSION['authorization'] = $user['opravneni'];
        return true;
    }
    else
        return false;
}
// ----------

// ----------
function delete_user($id)
{
    try {
        $stmt = $GLOBALS['pdo']->prepare('DELETE FROM Uzivatel WHERE id = ?');
        $stmt->execute(array($id));
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
}
// ----------

// ----------
function edit_user($id, $authorization)
{
    try {
        $stmt = $GLOBALS['pdo']->prepare('UPDATE Uzivatel SET opravneni = ? WHERE id = ?');
        $stmt->execute(array($authorization, $id));
    }
    catch (PDOException $e) {
        echo '<h2>Spojení s databází selhalo: ' .$e->getMessage().'</h2>';
    }
}
// ----------

// ----------
function has_rights_of($authorization)
{
    if (isset($_SESSION['authorization']))
        switch ($_SESSION['authorization'])
        {
            case 'administrator':
                if ($authorization == 'administrator') return true;
            case 'poradatel':
                if ($authorization == 'poradatel') return true;
            case 'pokladni':
                if ($authorization == 'pokladni') return true;
            case 'divak':
                if ($authorization == 'divak') return true;
            default:
                return false;
        }
    return false;
}
// ----------

// ----------
function alert($msg)
{
    echo "<script type='text/javascript'>alert('$msg');</script>";
}
// ----------

// omit closing tag for safety when importing
