using System;
using System.Data;
using System.Windows.Forms;
using Npgsql;

namespace WinFormsApp1
{
	public partial class ResultTableEditable : Form
	{
		private DataSet ds;
		private NpgsqlDataAdapter adapter;
		public ResultTableEditable(NpgsqlDataAdapter adapter)
		{
			InitializeComponent();
			ds = new DataSet();
			this.adapter = adapter;
			this.adapter.Fill(ds);
			dataGridView1.DataSource = ds.Tables[0];
		}

		private void CanselButton_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		private void SaveButton_Click(object sender, EventArgs e)
		{
			NpgsqlCommandBuilder commandBuilder = new(adapter);
			adapter.Update(ds);
		}
	}
}
