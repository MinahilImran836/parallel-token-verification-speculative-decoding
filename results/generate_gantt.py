import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime, timedelta

# Project timeline (June 2 - June 28, 2026)
tasks = [
    ('Sequential Baseline', '2026-06-02', '2026-06-05', 20),
    ('Pthreads Implementation', '2026-06-06', '2026-06-10', 40),
    ('OpenMP Implementation', '2026-06-11', '2026-06-14', 30),
    ('MPI Implementation', '2026-06-15', '2026-06-18', 35),
    ('Experiments & Testing', '2026-06-19', '2026-06-22', 25),
    ('Paper Writing', '2026-06-23', '2026-06-26', 30),
    ('Final Polish & Submission', '2026-06-27', '2026-06-28', 15),
]

fig, ax = plt.subplots(figsize=(12, 6))

# Convert dates to matplotlib format
start_dates = [datetime.strptime(d, '%Y-%m-%d') for _, d, _, _ in tasks]
end_dates = [datetime.strptime(d, '%Y-%m-%d') for _, _, d, _ in tasks]
durations = [(end - start).days for start, end in zip(start_dates, end_dates)]

# Create horizontal bars
for i, (task, start, end, hours) in enumerate(tasks):
    start_dt = datetime.strptime(start, '%Y-%m-%d')
    duration = (datetime.strptime(end, '%Y-%m-%d') - start_dt).days
    ax.barh(i, duration, left=start_dt, height=0.6, alpha=0.8, edgecolor='black')
    
    # Add task labels
    ax.text(start_dt + timedelta(days=duration/2), i, f'{hours}h', 
            ha='center', va='center', fontsize=9, fontweight='bold', color='white')

# Set y-axis labels
ax.set_yticks(range(len(tasks)))
ax.set_yticklabels([task for task, _, _, _ in tasks])

# Format x-axis
ax.xaxis_date()
ax.xaxis.set_major_locator(mdates.DayLocator(interval=2))
ax.xaxis.set_major_formatter(mdates.DateFormatter('%b %d'))
plt.xticks(rotation=45)

# Labels and title
ax.set_xlabel('Timeline (June 2 - June 28, 2026)', fontsize=12)
ax.set_ylabel('Tasks', fontsize=12)
ax.set_title('Project Gantt Chart: Parallel Token Verification Engine', fontsize=14, fontweight='bold')

# Grid
ax.grid(True, axis='x', alpha=0.3)

# Adjust layout
plt.tight_layout()

# Save
plt.savefig('gantt_chart.png', dpi=300, bbox_inches='tight')
print("Gantt chart saved to gantt_chart.png")
