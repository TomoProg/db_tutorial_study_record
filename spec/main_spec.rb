describe 'database' do
  def run_script(commands)
    raw_output = nil
    IO.popen('./db', 'r+') do |pipe|
      commands.each do |command|
        pipe.puts command
      end

      pipe.close_write

      raw_output = pipe.gets(nil)
    end
    raw_output.split("\n")
  end

  it 'insertした行がselectできること' do
    result = run_script([
      'insert 1 user1 person1@example.com',
      'select',
      '.exit',
    ])
    expect(result).to match_array([
      'db > Executed.',
      'db > (1, user1, person1@example.com)',
      'Executed.',
      'db > ',
    ])
  end

  it 'テーブルに空きがない場合エラーメッセージが出力されること' do
    script = (1..1401).map do |i|
      "insert #{i} user#{i} person#{i}@example.com"
    end
    script << ".exit"
    result = run_script(script)
    expect(result[-2]).to eq('db > Error: Table full.')
  end

  it '最大の文字列長でinsertできること' do
    long_username = 'a' * 32
    long_email = 'a' * 255
    script = [
      "insert 1 #{long_username} #{long_email}",
      'select',
      '.exit',
    ]

    result = run_script(script)
    expect(result).to match_array([
      'db > Executed.',
      "db > (1, #{long_username}, #{long_email})",
      'Executed.',
      'db > ',
    ])
  end

  it '最大の文字列長を超えていた場合はエラーメッセージが出力されること' do
    long_username = 'a' * 33 # 最大+1文字
    long_email = 'a' * 256   # 最大+1文字
    script = [
      "insert 1 #{long_username} #{long_email}",
      'select',
      '.exit',
    ]
    result = run_script(script)
    expect(result).to match_array([
      'db > String is too long.',
      'db > Executed.',
      'db > ',
    ])
  end

  it 'idが負の数だった場合はエラーメッセージが出力されること' do
    script = [
      'insert -1 cstack foo@bar.com',
      'select',
      '.exit',
    ]
    result = run_script(script)
    expect(result).to match_array([
      'db > ID must be positive.',
      'db > Executed.',
      'db > ',
    ])
  end
end
