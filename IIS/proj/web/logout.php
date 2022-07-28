<?php
    require 'functions.php';

    unset($_SESSION['authorization']);
    unset($_SESSION['user']);

    print_header('Odhlášeno');
    print_footer();
?>
