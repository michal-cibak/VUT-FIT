<?php
    require 'functions.php';

    print_header('Registrovat se');

    // register user if a registration form was sent
    if (array_key_exists('email', $_POST) && array_key_exists('username', $_POST) && array_key_exists('password', $_POST))
    {
        register_user();
        print_footer();
        die();
    }
?>
    <h1>Registrovat se</h1>
    <form method="post">
        <label for="username">Uživatelské jméno</label><br>
        <input type="text" name="username" id="username" required><br>
        <label for="email">E-mailová adresa</label><br>
        <input type="email" name="email" id="email" required><br>
        <label for="password">Heslo</label><br>
        <input type="password" name="password" id="password" required><br>
        <input type="submit" value="Registrovat">
    </form>
<?php
    print_footer()
?>
