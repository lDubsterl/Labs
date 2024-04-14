using System;
using System.Data;
using System.Windows.Forms;
using Npgsql;

namespace WinFormsApp1
{
	public partial class ResultTable : Form
	{
		public ResultTable(NpgsqlDataAdapter adapter)
		{
			InitializeComponent();
			var ds = new DataSet();
			adapter.Fill(ds);
			dataGridView1.DataSource = ds.Tables[0];
		}
	}
}
