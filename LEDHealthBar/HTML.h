    String html = R"(

      <h1 style='text-align: center;'><span style='color: #ff0000;'><strong>Player HP Tracker</strong></span></h1>
      <form method='post'>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 8px;'>
      <td style='width: 40.5622%; height: 8px; text-align: center;'>Brightness</td> 
      <td style='width: 59.4377%; height: 8px; text-align: center;'><input style='text-align: center;'id='b' name='b' size='3' type='text' value='$b' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>
      
      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;' id='p1' name='p1' size='5' type='text' value='$p1' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c1' name='c1' size='3' type='text' value='$c1' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m1' name='m1' size='3' type='text' value='$m1' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;'id='p2' name='p2' size='5' type='text' value='$p2' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c2' name='c2' size='3' type='text' value='$c2' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m2' name='m2' size='3' type='text' value='$m2' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;'id='p3' name='p3' size='5' type='text' value='$p3' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c3' name='c3' size='3' type='text' value='$c3' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m3' name='m3' size='3' type='text' value='$m3' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;'id='p4' name='p4' size='5' type='text' value='$p4' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c4' name='c4' size='3' type='text' value='$c4' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m4' name='m4' size='3' type='text' value='$m4' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <p style='text-align: center;'><input style='text-align: center;'type='submit' onsubmit='return false' value='Update' /></p>

      </form>
      </body>
      </html>)";

      #define SITE_HTML html