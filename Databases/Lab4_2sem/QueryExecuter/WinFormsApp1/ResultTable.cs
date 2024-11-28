using System.Data;
using System.Windows.Forms;

namespace WinFormsApp1
{
	public partial class ResultTable : Form
	{
		public ResultTable(DataTable dt)
		{
			InitializeComponent();
			dataGridView1.DataSource = dt;
		}
	}
}
