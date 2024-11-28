using MongoDB.Driver;
using System;
using System.Data;
using System.Windows.Forms;

namespace WinFormsApp1
{
	public partial class ResultTableEditable : Form
	{
		public ResultTableEditable(DataTable dt)
		{
			InitializeComponent();
			dataGridView1.DataSource = dt;
		}

		private void CanselButton_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		private void SaveButton_Click(object sender, EventArgs e)
		{
			//NpgsqlCommandBuilder commandBuilder = new(adapter);
			//adapter.Update(ds);
		}
	}
}
