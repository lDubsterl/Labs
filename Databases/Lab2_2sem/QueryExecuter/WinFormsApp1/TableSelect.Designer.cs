
namespace WinFormsApp1
{
	partial class TableSelect
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.placeButton = new System.Windows.Forms.Button();
			this.elevatorButton = new System.Windows.Forms.Button();
			this.clientTableButton = new System.Windows.Forms.Button();
			this.autoButton = new System.Windows.Forms.Button();
			this.floorButton = new System.Windows.Forms.Button();
			this.tableButton = new System.Windows.Forms.Button();
			this.floorElevatorButton = new System.Windows.Forms.Button();
			this.clientButton = new System.Windows.Forms.Button();
			this.tableLayoutPanel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.AutoSize = true;
			this.tableLayoutPanel1.ColumnCount = 2;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel1.Controls.Add(this.placeButton, 0, 1);
			this.tableLayoutPanel1.Controls.Add(this.elevatorButton, 0, 2);
			this.tableLayoutPanel1.Controls.Add(this.clientTableButton, 0, 3);
			this.tableLayoutPanel1.Controls.Add(this.autoButton, 1, 0);
			this.tableLayoutPanel1.Controls.Add(this.floorButton, 1, 1);
			this.tableLayoutPanel1.Controls.Add(this.tableButton, 1, 2);
			this.tableLayoutPanel1.Controls.Add(this.floorElevatorButton, 1, 3);
			this.tableLayoutPanel1.Controls.Add(this.clientButton, 0, 0);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 4;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(432, 294);
			this.tableLayoutPanel1.TabIndex = 0;
			// 
			// placeButton
			// 
			this.placeButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.placeButton.Location = new System.Drawing.Point(3, 76);
			this.placeButton.Name = "placeButton";
			this.placeButton.Size = new System.Drawing.Size(210, 67);
			this.placeButton.TabIndex = 1;
			this.placeButton.Text = "Места";
			this.placeButton.UseVisualStyleBackColor = true;
			this.placeButton.Click += new System.EventHandler(this.PlaceButton_Click);
			// 
			// elevatorButton
			// 
			this.elevatorButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.elevatorButton.Location = new System.Drawing.Point(3, 149);
			this.elevatorButton.Name = "elevatorButton";
			this.elevatorButton.Size = new System.Drawing.Size(210, 67);
			this.elevatorButton.TabIndex = 2;
			this.elevatorButton.Text = "Лифты";
			this.elevatorButton.UseVisualStyleBackColor = true;
			this.elevatorButton.Click += new System.EventHandler(this.ElevatorButton_Click);
			// 
			// clientTableButton
			// 
			this.clientTableButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.clientTableButton.Location = new System.Drawing.Point(3, 222);
			this.clientTableButton.Name = "clientTableButton";
			this.clientTableButton.Size = new System.Drawing.Size(210, 69);
			this.clientTableButton.TabIndex = 3;
			this.clientTableButton.Text = "Клиенты-Табло";
			this.clientTableButton.UseVisualStyleBackColor = true;
			this.clientTableButton.Click += new System.EventHandler(this.ClientTableButton_Click);
			// 
			// autoButton
			// 
			this.autoButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.autoButton.Location = new System.Drawing.Point(219, 3);
			this.autoButton.Name = "autoButton";
			this.autoButton.Size = new System.Drawing.Size(210, 67);
			this.autoButton.TabIndex = 4;
			this.autoButton.Text = "Автомобили";
			this.autoButton.UseVisualStyleBackColor = true;
			this.autoButton.Click += new System.EventHandler(this.AutoButton_Click);
			// 
			// floorButton
			// 
			this.floorButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.floorButton.Location = new System.Drawing.Point(219, 76);
			this.floorButton.Name = "floorButton";
			this.floorButton.Size = new System.Drawing.Size(210, 67);
			this.floorButton.TabIndex = 5;
			this.floorButton.Text = "Этажи";
			this.floorButton.UseVisualStyleBackColor = true;
			this.floorButton.Click += new System.EventHandler(this.FloorButton_Click);
			// 
			// tableButton
			// 
			this.tableButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableButton.Location = new System.Drawing.Point(219, 149);
			this.tableButton.Name = "tableButton";
			this.tableButton.Size = new System.Drawing.Size(210, 67);
			this.tableButton.TabIndex = 6;
			this.tableButton.Text = "Табло";
			this.tableButton.UseVisualStyleBackColor = true;
			this.tableButton.Click += new System.EventHandler(this.TableButton_Click);
			// 
			// floorElevatorButton
			// 
			this.floorElevatorButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.floorElevatorButton.Location = new System.Drawing.Point(219, 222);
			this.floorElevatorButton.Name = "floorElevatorButton";
			this.floorElevatorButton.Size = new System.Drawing.Size(210, 69);
			this.floorElevatorButton.TabIndex = 7;
			this.floorElevatorButton.Text = "Этажи-Лифты";
			this.floorElevatorButton.UseVisualStyleBackColor = true;
			this.floorElevatorButton.Click += new System.EventHandler(this.FloorElevatorButton_Click);
			// 
			// clientButton
			// 
			this.clientButton.Dock = System.Windows.Forms.DockStyle.Fill;
			this.clientButton.Location = new System.Drawing.Point(3, 3);
			this.clientButton.Name = "clientButton";
			this.clientButton.Size = new System.Drawing.Size(210, 67);
			this.clientButton.TabIndex = 0;
			this.clientButton.Text = "Клиенты";
			this.clientButton.UseVisualStyleBackColor = true;
			this.clientButton.Click += new System.EventHandler(this.ClientButton_Click);
			// 
			// TableSelect
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(432, 294);
			this.Controls.Add(this.tableLayoutPanel1);
			this.Name = "TableSelect";
			this.Text = "TableSelect";
			this.tableLayoutPanel1.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private System.Windows.Forms.Button placeButton;
		private System.Windows.Forms.Button elevatorButton;
		private System.Windows.Forms.Button clientTableButton;
		private System.Windows.Forms.Button autoButton;
		private System.Windows.Forms.Button floorButton;
		private System.Windows.Forms.Button tableButton;
		private System.Windows.Forms.Button floorElevatorButton;
		private System.Windows.Forms.Button clientButton;
	}
}