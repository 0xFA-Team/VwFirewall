/*
	��ӡͷ��
*/
function PrintHeader( strTitle )
{
	/*
		strTitle	- ���⣬���磺��������ר�Ҽ�顱
	*/
	var strHtml = ""
	+ "<table border=0 width=100%>"
	+ "  <tr>"
	+ "    <td height=25><a href=\"http://www.vidun.com/\" target=_blank style=\"color:#000000;text-decoration:none;\"><img src=\"images/vwlogo_new_h26.gif\" border=0 align=absmiddle />  <font color=666666 size=3><b><i>΢�ܷ���ǽ</i></b></font></td>"
	+ "    <td align=right valign=bottom>"
	+ ( -1 != location.href.indexOf( "http://" ) ? "<a href=\"http://rd.vidun.net/?4504\">����֧������</a> <b><font style=\"font-size:16px;\">&raquo;</font></b> " : "" )
	+ strTitle
	+ "</td>"
	+ "  </tr>"
	+ "  <tr>"
	+ "    <td colspan=2><hr></td>"
	+ "  </tr>"
	+ "  <tr>"
	+ "    <td colspan=2 height=10 valign=top></td>"
	+ "  </tr>"
	+ "</table>";
	
	document.write( strHtml );
}